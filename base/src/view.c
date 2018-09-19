#include <stdlib.h> // malloc free
#include <string.h> // strdup, memcmp, memcpy

#include "shoveler/hash.h"
#include "shoveler/view.h"

guint qualifiedComponentHash(gconstpointer qualifiedComponentPointer);
gboolean qualifiedComponentEqual(gconstpointer firstQualifiedComponentPointer, gconstpointer secondQualifiedComponentPointer);
static void triggerComponentCallback(ShovelerViewComponent *component, ShovelerViewComponentCallbackType callbackType);
static void freeEntity(void *entityPointer);
static void freeComponent(void *componentPointer);
static void freeCallbacks(void *callbacksPointer);
static void freeDependency(void *dependencyPointer);

ShovelerView *shovelerViewCreate()
{
	ShovelerView *view = malloc(sizeof(ShovelerView));
	view->entities = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, freeEntity);
	view->targets = g_hash_table_new_full(&g_str_hash, &g_str_equal, &free, NULL);
	view->dependencies = g_hash_table_new_full(qualifiedComponentHash, qualifiedComponentEqual, NULL, freeDependency);

	return view;
}

bool shovelerViewAddEntity(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = malloc(sizeof(ShovelerViewEntity));
	entity->view = view;
	entity->entityId = entityId;
	entity->components = g_hash_table_new_full(&g_str_hash, &g_str_equal, NULL, &freeComponent);
	entity->callbacks = g_hash_table_new_full(&g_str_hash, &g_str_equal, &free, &freeCallbacks);

	if(!g_hash_table_insert(view->entities, &entity->entityId, entity)) {
		freeEntity(entity);
		return false;
	} else {
		return true;
	}
}

bool shovelerViewRemoveEntity(ShovelerView *view, long long int entityId)
{
	return g_hash_table_remove(view->entities, &entityId);
}

bool shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentName, void *data, ShovelerViewComponentFreeFunction *freeFunction)
{
	ShovelerViewComponent *component = malloc(sizeof(ShovelerViewComponent));
	component->entity = entity;
	component->name = strdup(componentName);
	component->data = data;
	component->active = false;
	component->authoritative = false;
	component->dependencies = g_queue_new();
	component->free = freeFunction;

	if(!g_hash_table_insert(entity->components, component->name, component)) {
		freeComponent(component);
		return false;
	}

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_ADD);
	return true;
}

bool shovelerViewEntityUpdateComponent(ShovelerViewEntity *entity, const char *componentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_UPDATE);
	return true;
}

bool shovelerViewDelegateComponent(ShovelerViewEntity *entity, const char *componentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	component->authoritative = true;

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_DELEGATE);
	return true;
}

bool shovelerViewUndelegateComponent(ShovelerViewEntity *entity, const char *componentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	component->authoritative = false;

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_UNDELEGATE);
	return true;
}

bool shovelerViewEntityAddComponentDependency(ShovelerViewEntity *entity, const char *componentName, long long int dependencyEntityId, const char *dependencyComponentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	ShovelerViewDependency *dependency = malloc(sizeof(ShovelerViewDependency));
	dependency->sourceComponent.entityId = entity->entityId;
	dependency->sourceComponent.componentName = strdup(component->name);
	dependency->targetComponent.entityId = dependencyEntityId;
	dependency->targetComponent.componentName = strdup(dependencyComponentName);

	g_queue_push_tail(component->dependencies, &dependency->targetComponent);

	if(!g_hash_table_insert(entity->view->dependencies, &dependency->targetComponent, &dependency)) {
		g_queue_pop_tail(component->dependencies);
		freeDependency(dependency);
		return false;
	}
	return true;
}

bool shovelerViewEntityRemoveComponent(ShovelerViewEntity *entity, const char *componentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_REMOVE);
	return g_hash_table_remove(entity->components, componentName);
}

ShovelerViewComponentCallback *shovelerViewEntityAddCallback(ShovelerViewEntity *entity, const char *componentName, ShovelerViewComponentCallbackFunction *function, void *userData)
{
	GQueue *callbacks = g_hash_table_lookup(entity->callbacks, componentName);
	if(callbacks == NULL) {
		callbacks = g_queue_new();
		g_hash_table_insert(entity->callbacks, strdup(componentName), callbacks);
	}

	ShovelerViewComponentCallback *callback = malloc(sizeof(ShovelerViewComponentCallback));
	callback->function = function;
	callback->userData = userData;
	g_queue_push_tail(callbacks, callback);

	// if the target component is already there, trigger the add callback directly
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component != NULL) {
		callback->function(component, SHOVELER_VIEW_COMPONENT_CALLBACK_ADD, callback->userData);
	}

	return callback;
}

bool shovelerViewSetTarget(ShovelerView *view, const char *targetName, void *target)
{
	return g_hash_table_insert(view->targets, strdup(targetName), target);
}

bool shovelerViewEntityRemoveCallback(ShovelerViewEntity *entity, const char *componentName, ShovelerViewComponentCallback *callback)
{
	GQueue *callbacks = g_hash_table_lookup(entity->callbacks, componentName);
	if(callbacks == NULL) {
		return false;
	}

	if(!g_queue_remove(callbacks, callback)) {
		return false;
	}

	free(callback);
	return true;
}

void shovelerViewFree(ShovelerView *view)
{
	g_hash_table_destroy(view->entities);
	g_hash_table_destroy(view->targets);
	g_hash_table_destroy(view->dependencies);
	free(view);
}

guint qualifiedComponentHash(gconstpointer qualifiedComponentPointer)
{
	ShovelerViewQualifiedComponent *qualifiedComponent = (ShovelerViewQualifiedComponent *) qualifiedComponentPointer;

	guint entityIdHash = g_int64_hash(&qualifiedComponent->entityId);
	guint componentNameHash = g_str_hash(qualifiedComponent->componentName);

	return shovelerHashCombine(entityIdHash, componentNameHash);
}

gboolean qualifiedComponentEqual(gconstpointer firstQualifiedComponentPointer, gconstpointer secondQualifiedComponentPointer)
{
	return memcmp(firstQualifiedComponentPointer, secondQualifiedComponentPointer, sizeof(ShovelerViewQualifiedComponent)) == 0;
}

static void triggerComponentCallback(ShovelerViewComponent *component, ShovelerViewComponentCallbackType callbackType)
{
	GQueue *callbacks = g_hash_table_lookup(component->entity->callbacks, component->name);
	if(callbacks != NULL) {
		for(GList *iter = callbacks->head; iter != NULL; iter = iter->next) {
			ShovelerViewComponentCallback *callback = iter->data;
			callback->function(component, callbackType, callback->userData);
		}
	}
}

static void freeEntity(void *entityPointer)
{
	ShovelerViewEntity *entity = entityPointer;
	g_hash_table_destroy(entity->components);
	g_hash_table_destroy(entity->callbacks);
	free(entity);
}

static void freeComponent(void *componentPointer)
{
	ShovelerViewComponent *component = componentPointer;
	component->free(component);
	g_queue_free(component->dependencies);
	free(component->name);
	free(component);
}

static void freeCallbacks(void *callbacksPointer)
{
	GQueue *callbacks = callbacksPointer;
	g_queue_free(callbacks);
}

static void freeDependency(void *dependencyPointer)
{
	ShovelerViewDependency *dependency = (ShovelerViewDependency *) dependencyPointer;

	free(dependency->sourceComponent.componentName);
	free(dependency->targetComponent.componentName);
	free(dependency);
}
