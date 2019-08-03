#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // strdup, memcmp, memcpy

#include "shoveler/component.h"
#include "shoveler/file.h"
#include "shoveler/hash.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

guint qualifiedComponentHash(gconstpointer qualifiedComponentPointer);
gboolean qualifiedComponentEqual(gconstpointer firstQualifiedComponentPointer, gconstpointer secondQualifiedComponentPointer);
static void triggerComponentCallback(ShovelerViewComponent *component, ShovelerViewComponentCallbackType callbackType);
static ShovelerViewQualifiedComponent *copyQualifiedComponent(ShovelerViewQualifiedComponent *qualifiedComponent);
static bool removeDependency(GQueue *dependencies, long long int dependencyEntityId, const char *dependencyComponentName);
static bool activateTypedComponent(ShovelerViewComponent *viewComponent, void *componentPointer);
static void deactivateTypedComponent(ShovelerViewComponent *viewComponent, void *componentPointer);
static void freeTypedComponent(ShovelerViewComponent *viewComponent, void *componentPointer);
static void freeComponentType(void *componentTypePointer);
static void freeEntity(void *entityPointer);
static void freeComponent(void *componentPointer);
static void freeCallbacks(void *callbacksPointer);
static void freeQualifiedComponents(void *qualifiedComponentsPointer);
static void freeQualifiedComponent(void *qualifiedComponentPointer);

ShovelerView *shovelerViewCreate()
{
	ShovelerView *view = malloc(sizeof(ShovelerView));
	view->componentTypes = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeComponentType);
	view->entities = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, freeEntity);
	view->targets = g_hash_table_new_full(&g_str_hash, &g_str_equal, &free, NULL);
	view->reverseDependencies = g_hash_table_new_full(qualifiedComponentHash, qualifiedComponentEqual, freeQualifiedComponent, freeQualifiedComponents);
	view->dependencyCallbacks = g_queue_new();
	view->numEntities = 0;
	view->numComponents = 0;
	view->numComponentDependencies = 0;
	view->numActiveComponents = 0;
	view->numDelegatedComponents = 0;

	return view;
}

bool shovelerViewAddComponentType(ShovelerView *view, ShovelerComponentType *componentType)
{
	return g_hash_table_insert(view->componentTypes, componentType->name, componentType);
}

bool shovelerViewHasComponentType(ShovelerView *view, const char *componentTypeName)
{
	return g_hash_table_lookup(view->componentTypes, componentTypeName) != NULL;
}

ShovelerViewEntity *shovelerViewAddEntity(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = malloc(sizeof(ShovelerViewEntity));
	entity->view = view;
	entity->entityId = entityId;
	entity->type = NULL;
	entity->components = g_hash_table_new_full(&g_str_hash, &g_str_equal, NULL, &freeComponent);
	entity->callbacks = g_hash_table_new_full(&g_str_hash, &g_str_equal, &free, &freeCallbacks);

	if(!g_hash_table_insert(view->entities, &entity->entityId, entity)) {
		freeEntity(entity);
		return NULL;
	}

	view->numEntities++;
	shovelerLogTrace("Added entity %lld.", entity->entityId);
	return entity;
}

bool shovelerViewRemoveEntity(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = g_hash_table_lookup(view->entities, &entityId);
	if(entity == NULL) {
		return false;
	}

	GList *componentNames = g_hash_table_get_keys(entity->components);
	for(GList *iter = componentNames; iter != NULL; iter = iter->next) {
		char *componentName = iter->data;
		shovelerViewEntityRemoveComponent(entity, componentName);
	}
	g_list_free(componentNames);

	assert(g_hash_table_size(entity->components) == 0);

	if(!g_hash_table_remove(view->entities, &entityId)) {
		return false;
	}

	view->numEntities--;
	shovelerLogTrace("Removed entity %lld.", entityId);
	return true;
}

void shovelerViewEntitySetType(ShovelerViewEntity *entity, const char *type)
{
	free(entity->type);

	if(type != NULL) {
		entity->type = strdup(type);
	} else {
		entity->type = NULL;
	}
}

ShovelerViewComponent *shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentName, void *data, ShovelerViewComponentActivateFunction *activate, ShovelerViewComponentDeactivateFunction *deactivate, ShovelerViewComponentFreeFunction *freeFunction)
{
	ShovelerViewComponent *component = malloc(sizeof(ShovelerViewComponent));
	component->entity = entity;
	component->name = strdup(componentName);
	component->data = data;
	component->active = false;
	component->authoritative = false;
	component->dependencies = g_queue_new();
	component->activate = activate;
	component->deactivate = deactivate;
	component->free = freeFunction;

	if(!g_hash_table_insert(entity->components, component->name, component)) {
		freeComponent(component);
		return NULL;
	}

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_ADD);

	entity->view->numComponents++;
	shovelerLogTrace("Added component '%s' to entity %lld.", componentName, entity->entityId);
	return component;
}

ShovelerViewComponent *shovelerViewEntityAddTypedComponent(ShovelerViewEntity *entity, const char *componentTypeName)
{
	ShovelerComponentType *componentType = g_hash_table_lookup(entity->view->componentTypes, componentTypeName);
	if(componentType == NULL) {
		return NULL;
	}

	ShovelerComponent *component = shovelerComponentCreate(componentType, /* callbackUserData */ NULL);
	ShovelerViewComponent *viewComponent = shovelerViewEntityAddComponent(entity, componentType->name, component, activateTypedComponent, deactivateTypedComponent, freeTypedComponent);
	if(viewComponent == NULL) {
		shovelerComponentFree(component);
		return NULL;
	}

	component->callbackUserData = viewComponent;
	return viewComponent;
}

ShovelerComponent *shovelerViewEntityGetTypedComponent(ShovelerViewEntity *entity, const char *componentTypeName)
{
	ShovelerViewComponent *viewComponent = shovelerViewEntityGetComponent(entity, componentTypeName);
	if(viewComponent == NULL) {
		return NULL;
	}

	return viewComponent->data;
}

void shovelerViewComponentUpdate(ShovelerViewComponent *component)
{
	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_UPDATE);
}

void shovelerViewComponentDelegate(ShovelerViewComponent *component)
{
	component->authoritative = true;
	component->entity->view->numDelegatedComponents++;
	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_DELEGATE);
}

void shovelerViewComponentUndelegate(ShovelerViewComponent *component)
{
	component->authoritative = false;
	component->entity->view->numDelegatedComponents--;
	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_UNDELEGATE);
}

void shovelerViewComponentAddDependency(ShovelerViewComponent *component, long long int dependencyEntityId, const char *dependencyComponentName)
{
	ShovelerViewQualifiedComponent *dependencySource = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencySource->entityId = component->entity->entityId;
	dependencySource->componentName = strdup(component->name);

	ShovelerViewQualifiedComponent *dependencyTarget = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencyTarget->entityId = dependencyEntityId;
	dependencyTarget->componentName = strdup(dependencyComponentName);

	GQueue *reverseDependencies = g_hash_table_lookup(component->entity->view->reverseDependencies, dependencyTarget);
	if(reverseDependencies == NULL) {
		ShovelerViewQualifiedComponent *reverseDependenciesKey = copyQualifiedComponent(dependencyTarget);
		reverseDependencies = g_queue_new();
		g_hash_table_insert(component->entity->view->reverseDependencies, reverseDependenciesKey, reverseDependencies);
	}

	g_queue_push_tail(component->dependencies, dependencyTarget);
	g_queue_push_tail(reverseDependencies, dependencySource);

	component->entity->view->numComponentDependencies++;
	shovelerLogTrace("Added component dependency on component '%s' of entity %lld to component '%s' of entity %lld.", dependencyComponentName, dependencyEntityId, component->name, component->entity->entityId);

	// if the component is already active, we need to check if the new dependency is active and deactivate in case it isn't
	if(component->active) {
		bool needsDeactivation = true;
		ShovelerViewEntity *dependencyEntity = g_hash_table_lookup(component->entity->view->entities, &dependencyTarget->entityId);
		if(dependencyEntity != NULL) {
			ShovelerViewComponent *dependencyComponent = g_hash_table_lookup(dependencyEntity->components, &dependencyTarget->componentName);
			if(dependencyComponent != NULL) {
				if(dependencyComponent->active) {
					needsDeactivation = false;
				}
			}
		}

		if(needsDeactivation) {
			shovelerViewComponentDeactivate(component);
		}
	}

	for(GList *iter = component->entity->view->dependencyCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerViewDependencyCallback *callback = iter->data;
		callback->function(component->entity->view, dependencySource, dependencyTarget, true, callback->userData);
	}
}

bool shovelerViewComponentRemoveDependency(ShovelerViewComponent *component, long long int dependencyEntityId, const char *dependencyComponentName)
{
	ShovelerViewQualifiedComponent dependencyTarget;
	dependencyTarget.entityId = dependencyEntityId;
	dependencyTarget.componentName = strdup(dependencyComponentName);

	GQueue *reverseDependencies = g_hash_table_lookup(component->entity->view->reverseDependencies, &dependencyTarget);
	assert(reverseDependencies != NULL);

	if(!removeDependency(reverseDependencies, component->entity->entityId, component->name)) {
		free(dependencyTarget.componentName);
		return false;
	}

	if(!removeDependency(component->dependencies, dependencyEntityId, dependencyTarget.componentName)) {
		free(dependencyTarget.componentName);
		return false;
	}

	component->entity->view->numComponentDependencies--;
	shovelerLogTrace("Removed component dependency on component '%s' of entity %lld from component '%s' of entity %lld.", dependencyTarget.componentName, dependencyEntityId, component->name, component->entity->entityId);

	ShovelerViewQualifiedComponent dependencySource;
	dependencySource.entityId = component->entity->entityId;
	dependencySource.componentName = component->name;

	for(GList *iter = component->entity->view->dependencyCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerViewDependencyCallback *callback = iter->data;
		callback->function(component->entity->view, &dependencySource, &dependencyTarget, false, callback->userData);
	}

	free(dependencyTarget.componentName);
	return true;
}

bool shovelerViewComponentActivate(ShovelerViewComponent *component)
{
	if(component->active) {
		return false;
	}

	// check if dependencies are active
	for(GList *iter = component->dependencies->head; iter != NULL; iter = iter->next) {
		ShovelerViewQualifiedComponent *dependencyTarget = iter->data;

		ShovelerViewEntity *dependencyEntity = g_hash_table_lookup(component->entity->view->entities, &dependencyTarget->entityId);
		if(dependencyEntity == NULL) {
			shovelerLogTrace("Couldn't activate component '%s' of entity %lld because dependency entity %lld is not in view.", component->name, component->entity->entityId, dependencyTarget->entityId);
			return false;
		}

		ShovelerViewComponent *dependencyComponent = g_hash_table_lookup(dependencyEntity->components, dependencyTarget->componentName);
		if(dependencyComponent == NULL) {
			shovelerLogTrace("Couldn't activate component '%s' of entity %lld because dependency entity %lld doesn't contain required component '%s'.", component->name, component->entity->entityId, dependencyTarget->entityId, dependencyTarget->componentName);
			return false;
		}

		if(!dependencyComponent->active) {
			shovelerLogTrace("Couldn't activate component '%s' of entity %lld because dependency component '%s' of entity %lld isn't active.", component->name, component->entity->entityId, dependencyTarget->componentName, dependencyTarget->entityId);
			return false;
		}
	}

	// activate the component
	if(component->activate != NULL) {
		if(!component->activate(component, component->data)) {
			shovelerLogTrace("Failed to activate component '%s' of entity %lld.", component->name, component->entity->entityId);
			return false;
		}
	}
	component->active = true;

	component->entity->view->numActiveComponents++;
	shovelerLogTrace("Activated component '%s' of entity %lld.", component->name, component->entity->entityId);

	// activate reverse dependencies
	ShovelerViewQualifiedComponent qualifiedComponent;
	qualifiedComponent.entityId = component->entity->entityId;
	qualifiedComponent.componentName = component->name;

	GQueue *reverseDependencies = g_hash_table_lookup(component->entity->view->reverseDependencies, &qualifiedComponent);
	if(reverseDependencies != NULL) {
		for(GList *iter = reverseDependencies->head; iter != NULL; iter = iter->next) {
			ShovelerViewQualifiedComponent *reverseDependency = iter->data;

			ShovelerViewEntity *reverseDependencyEntity = g_hash_table_lookup(component->entity->view->entities, &reverseDependency->entityId);
			assert(reverseDependencyEntity != NULL);

			ShovelerViewComponent *reverseDependencyComponent = g_hash_table_lookup(reverseDependencyEntity->components, reverseDependency->componentName);
			assert(reverseDependencyComponent != NULL);

			// An earlier reverse dependency could have already recursively activated the component, so only try to activate if necessary.
			if(!reverseDependencyComponent->active) {
				shovelerViewComponentActivate(reverseDependencyComponent);
			}
		}
	}

	return true;
}

void shovelerViewComponentDeactivate(ShovelerViewComponent *component)
{
	if(!component->active) {
		return;
	}

	// deactivate reverse dependencies
	ShovelerViewQualifiedComponent qualifiedComponent;
	qualifiedComponent.entityId = component->entity->entityId;
	qualifiedComponent.componentName = component->name;

	GQueue *reverseDependencies = g_hash_table_lookup(component->entity->view->reverseDependencies, &qualifiedComponent);
	if(reverseDependencies != NULL) {
		for(GList *iter = reverseDependencies->head; iter != NULL; iter = iter->next) {
			ShovelerViewQualifiedComponent *reverseDependency = iter->data;

			ShovelerViewEntity *reverseDependencyEntity = g_hash_table_lookup(component->entity->view->entities, &reverseDependency->entityId);
			assert(reverseDependencyEntity != NULL);

			shovelerLogTrace("Deactivating component '%s' of entity %lld because its dependency component '%s' of entity %lld is being deactivated.", reverseDependency->componentName, reverseDependency->entityId, component->name, component->entity->entityId);

			ShovelerViewComponent *reverseDependencyComponent = g_hash_table_lookup(reverseDependencyEntity->components, reverseDependency->componentName);
			assert(reverseDependencyComponent != NULL);

			shovelerViewComponentDeactivate(reverseDependencyComponent);
		}
	}

	// deactivate the component
	component->active = false;

	if(component->deactivate != NULL) {
		component->deactivate(component, component->data);
	}

	component->entity->view->numActiveComponents--;
	shovelerLogTrace("Deactivated component '%s' of entity %lld.", component->name, component->entity->entityId);
}

bool shovelerViewEntityRemoveComponent(ShovelerViewEntity *entity, const char *componentName)
{
	ShovelerViewComponent *component = g_hash_table_lookup(entity->components, componentName);
	if(component == NULL) {
		return false;
	}

	if(component->active) {
		shovelerViewComponentDeactivate(component);
	}

	// copy dependencies queue because we are removing from it as we iterate over it
	GQueue *dependenciesCopy = g_queue_copy(component->dependencies);
	for(GList *iter = dependenciesCopy->head; iter != NULL; iter = iter->next) {
		ShovelerViewQualifiedComponent *dependencyTarget = iter->data;
		shovelerViewComponentRemoveDependency(component, dependencyTarget->entityId, dependencyTarget->componentName);
	}
	g_queue_free(dependenciesCopy);

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_REMOVE);

	component->entity->view->numComponents--;
	shovelerLogTrace("Removed component '%s' from entity %lld.", componentName, entity->entityId);

	g_hash_table_remove(entity->components, componentName);
	return true;
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

GString *shovelerViewCreateDependencyGraph(ShovelerView *view)
{
	GString *graph = g_string_new("");
	g_string_append(graph, "digraph G {\n");
	g_string_append(graph, "	rankdir = \"LR\";\n");

	GHashTableIter entityIter;
	long long int *entityIdPointer;
	ShovelerViewEntity *entity;
	g_hash_table_iter_init(&entityIter, view->entities);
	while(g_hash_table_iter_next(&entityIter, (gpointer *) &entityIdPointer, (gpointer *) &entity)) {
		long long int entityId = *entityIdPointer;

		g_string_append_printf(graph, "	subgraph cluster_entity%lld {\n", entityId);

		if(entity->type != NULL) {
			g_string_append_printf(graph, "		label = \"%s %lld\";\n", entity->type, entityId);
		} else {
			g_string_append_printf(graph, "		label = \"%lld\";\n", entityId);
		}

		GHashTableIter componentIter;
		const char *componentName;
		ShovelerViewComponent *component;
		g_hash_table_iter_init(&componentIter, entity->components);
		while(g_hash_table_iter_next(&componentIter, (gpointer *) &componentName, (gpointer *) &component)) {
			const char *color = component->active ? "green" : "red";
			g_string_append_printf(graph, "		entity%lld_%s [label = <<font color='%s'>%s</font>>];\n", entityId, componentName, color, componentName);
		}

		g_string_append(graph, "	}\n");

		g_hash_table_iter_init(&componentIter, entity->components);
		while(g_hash_table_iter_next(&componentIter, (gpointer *) &componentName, (gpointer *) &component)) {
			for(GList *dependencyIter = component->dependencies->head; dependencyIter != NULL; dependencyIter = dependencyIter->next) {
				const ShovelerViewQualifiedComponent *dependencyTarget = (ShovelerViewQualifiedComponent *) dependencyIter->data;
				g_string_append_printf(graph, "	entity%lld_%s -> entity%lld_%s;\n", entityId, componentName, dependencyTarget->entityId, dependencyTarget->componentName);
			}

		}
	}

	g_string_append(graph, "}\n");
	return graph;
}

bool shovelerViewWriteDependencyGraph(ShovelerView *view, const char *filename)
{
	GString *graph = shovelerViewCreateDependencyGraph(view);
	bool success = shovelerFileWriteString(filename, graph->str);
	g_string_free(graph, true);
	return success;
}

ShovelerViewDependencyCallback *shovelerViewAddDependencyCallback(ShovelerView *view, ShovelerViewDependencyCallbackFunction *function, void *userData)
{
	ShovelerViewDependencyCallback *callback = malloc(sizeof(ShovelerViewDependencyCallback));
	callback->function = function;
	callback->userData = userData;
	g_queue_push_tail(view->dependencyCallbacks, callback);

	return callback;
}

bool shovelerViewRemoveDependencyCallback(ShovelerView *view, ShovelerViewDependencyCallback *callback)
{
	if(!g_queue_remove(view->dependencyCallbacks, callback)) {
		return false;
	}

	free(callback);
	return true;
}

void shovelerViewFree(ShovelerView *view)
{
	g_queue_free_full(view->dependencyCallbacks, free);
	g_hash_table_destroy(view->entities);
	g_hash_table_destroy(view->targets);
	g_hash_table_destroy(view->reverseDependencies);
	g_hash_table_destroy(view->componentTypes);
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
	ShovelerViewQualifiedComponent *firstQualifiedComponent = (ShovelerViewQualifiedComponent *) firstQualifiedComponentPointer;
	ShovelerViewQualifiedComponent *secondQualifiedComponent = (ShovelerViewQualifiedComponent *) secondQualifiedComponentPointer;

	return firstQualifiedComponent->entityId == secondQualifiedComponent->entityId
		&& strcmp(firstQualifiedComponent->componentName, secondQualifiedComponent->componentName) == 0;
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

static bool removeDependency(GQueue *dependencies, long long int dependencyEntityId, const char *dependencyComponentName)
{
	for(GList *iter = dependencies->head; iter != NULL; iter = iter->next) {
		ShovelerViewQualifiedComponent *dependencyTarget = iter->data;
		if(dependencyTarget->entityId == dependencyEntityId && strcmp(dependencyTarget->componentName, dependencyComponentName) == 0) {
			g_queue_delete_link(dependencies, iter);
			freeQualifiedComponent(dependencyTarget);
			return true;
		}
	}

	return false;
}

static bool activateTypedComponent(ShovelerViewComponent *viewComponent, void *componentPointer)
{
	ShovelerComponent *component = componentPointer;
	return shovelerComponentActivate(component);
}

static void deactivateTypedComponent(ShovelerViewComponent *viewComponent, void *componentPointer)
{
	ShovelerComponent *component = componentPointer;
	shovelerComponentDeactivate(component);
}

static void freeTypedComponent(ShovelerViewComponent *viewComponent, void *componentPointer)
{
	ShovelerComponent *component = componentPointer;
	shovelerComponentFree(component);
}

static void freeComponentType(void *componentTypePointer)
{
	ShovelerComponentType *componentType = componentTypePointer;
	shovelerComponentTypeFree(componentType);
}

static void freeEntity(void *entityPointer)
{
	ShovelerViewEntity *entity = entityPointer;
	g_hash_table_destroy(entity->components);
	g_hash_table_destroy(entity->callbacks);
	free(entity->type);
	free(entity);
}

static void freeComponent(void *componentPointer)
{
	ShovelerViewComponent *component = componentPointer;

	if(component->free != NULL) {
		component->free(component, component->data);
	}

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
