#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // strdup, memcmp, memcpy

#include "shoveler/hash.h"
#include "shoveler/view.h"

guint qualifiedComponentHash(gconstpointer qualifiedComponentPointer);
gboolean qualifiedComponentEqual(gconstpointer firstQualifiedComponentPointer, gconstpointer secondQualifiedComponentPointer);
static void triggerComponentCallback(ShovelerViewComponent *component, ShovelerViewComponentCallbackType callbackType);
static ShovelerViewQualifiedComponent *copyQualifiedComponent(ShovelerViewQualifiedComponent *qualifiedComponent);
static void freeEntity(void *entityPointer);
static void freeComponent(void *componentPointer);
static void freeCallbacks(void *callbacksPointer);
static void freeQualifiedComponents(void *qualifiedComponentsPointer);
static void freeQualifiedComponent(void *qualifiedComponentPointer);

ShovelerView *shovelerViewCreate()
{
	ShovelerView *view = malloc(sizeof(ShovelerView));
	view->entities = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, freeEntity);
	view->targets = g_hash_table_new_full(&g_str_hash, &g_str_equal, &free, NULL);
	view->reverseDependencies = g_hash_table_new_full(qualifiedComponentHash, qualifiedComponentEqual, freeQualifiedComponent, freeQualifiedComponents);

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

	if(component->active) {
		return false;
	}

	ShovelerViewQualifiedComponent *dependencySource = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencySource->entityId = entity->entityId;
	dependencySource->componentName = strdup(componentName);

	ShovelerViewQualifiedComponent *dependencyTarget = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencyTarget->entityId = dependencyEntityId;
	dependencyTarget->componentName = strdup(dependencyComponentName);

	GQueue *reverseDependencies = g_hash_table_lookup(entity->view->reverseDependencies, dependencyTarget);
	if(reverseDependencies == NULL) {
		ShovelerViewQualifiedComponent *reverseDependenciesKey = copyQualifiedComponent(dependencyTarget);
		reverseDependencies = g_queue_new();
		g_hash_table_insert(entity->view->reverseDependencies, reverseDependenciesKey, reverseDependencies);
	}

	g_queue_push_tail(component->dependencies, dependencyTarget);
	g_queue_push_tail(reverseDependencies, dependencySource);
	return true;
}

bool shovelerViewEntityActivateComponent(ShovelerViewEntity *entity, const char *componentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	if(component->active) {
		return false;
	}

	// check if dependencies are active
	for(GList *iter = component->dependencies->head; iter != NULL; iter = iter->next) {
		ShovelerViewQualifiedComponent *dependencyTarget = iter->data;

		ShovelerViewEntity *dependencyEntity = g_hash_table_lookup(entity->view->entities, &dependencyTarget->entityId);
		if(dependencyEntity == NULL) {
			return false;
		}

		ShovelerViewComponent *dependencyComponent = g_hash_table_lookup(dependencyEntity->components, &dependencyTarget->componentName);
		if(dependencyComponent == NULL) {
			return false;
		}

		if(dependencyComponent->active) {
			return false;
		}
	}

	// activate the component
	component->active = true;

	if(component->activate != NULL) {
		component->activate(component->data);
	}

	// activate reverse dependencies
	ShovelerViewQualifiedComponent qualifiedComponent;
	qualifiedComponent.entityId = entity->entityId;
	qualifiedComponent.componentName = component->name;

	GQueue *reverseDependencies = g_hash_table_lookup(entity->view->reverseDependencies, &qualifiedComponent);
	if(reverseDependencies != NULL) {
		for(GList *iter = reverseDependencies->head; iter != NULL; iter = iter->next) {
			ShovelerViewQualifiedComponent *reverseDependency = iter->data;

			ShovelerViewEntity *reverseDependencyEntity = g_hash_table_lookup(entity->view->entities, &reverseDependency->entityId);
			assert(reverseDependencyEntity != NULL);

			ShovelerViewComponent *reverseDependencyComponent = g_hash_table_lookup(reverseDependencyEntity->components, reverseDependency->componentName);
			assert(reverseDependencyComponent != NULL);
			assert(!reverseDependencyComponent->active);

			shovelerViewEntityActivateComponent(reverseDependencyEntity, reverseDependency->componentName);
		}
	}

	return true;
}

bool shovelerViewEntityDeactivateComponent(ShovelerViewEntity *entity, const char *componentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	if(!component->active) {
		return false;
	}

	// deactivate reverse dependencies
	ShovelerViewQualifiedComponent qualifiedComponent;
	qualifiedComponent.entityId = entity->entityId;
	qualifiedComponent.componentName = component->name;

	GQueue *reverseDependencies = g_hash_table_lookup(entity->view->reverseDependencies, &qualifiedComponent);
	if(reverseDependencies != NULL) {
		for(GList *iter = reverseDependencies->head; iter != NULL; iter = iter->next) {
			ShovelerViewQualifiedComponent *reverseDependency = iter->data;

			ShovelerViewEntity *reverseDependencyEntity = g_hash_table_lookup(entity->view->entities, &reverseDependency->entityId);
			assert(reverseDependencyEntity != NULL);

			if(!shovelerViewEntityDeactivateComponent(reverseDependencyEntity, reverseDependency->componentName)) {
				return false;
			}
		}
	}

	// deactivate the component
	component->active = false;

	if(component->deactivate != NULL) {
		component->deactivate(component->data);
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
	g_hash_table_destroy(view->reverseDependencies);
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

static ShovelerViewQualifiedComponent *copyQualifiedComponent(ShovelerViewQualifiedComponent *qualifiedComponent)
{
	ShovelerViewQualifiedComponent *copiedQualifiedComponent = malloc(sizeof(ShovelerViewQualifiedComponent));
	copiedQualifiedComponent->entityId = qualifiedComponent->entityId;
	copiedQualifiedComponent->componentName = strdup(qualifiedComponent->componentName);
	return copiedQualifiedComponent;
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
	g_queue_free_full(component->dependencies, freeQualifiedComponent);
	free(component->name);
	free(component);
}

static void freeCallbacks(void *callbacksPointer)
{
	GQueue *callbacks = callbacksPointer;
	g_queue_free(callbacks);
}

static void freeQualifiedComponents(void *qualifiedComponentsPointer)
{
	GQueue *qualifiedComponents = (GQueue *) qualifiedComponentsPointer;
	g_queue_free_full(qualifiedComponents, freeQualifiedComponent);
}

static void freeQualifiedComponent(void *qualifiedComponentPointer)
{
	ShovelerViewQualifiedComponent *qualifiedComponent = (ShovelerViewQualifiedComponent *) qualifiedComponentPointer;
	free(qualifiedComponent->componentName);
	free(qualifiedComponent);
}
