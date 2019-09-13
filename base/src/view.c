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
static void triggerComponentCallback(ShovelerComponent *component, ShovelerViewComponentCallbackType callbackType);
static ShovelerViewQualifiedComponent *copyQualifiedComponent(ShovelerViewQualifiedComponent *qualifiedComponent);
static bool checkDependency(ShovelerView *view, const ShovelerViewQualifiedComponent *dependencyTarget);
static bool removeDependency(GQueue *dependencies, const ShovelerViewQualifiedComponent *dependency);
static void activateReverseDependency(ShovelerView *view, long long int targetEntityId, const char *targetComponentTypeName, ShovelerComponent *sourceComponent, void *unused);
static void deactivateReverseDependency(ShovelerView *view, long long int targetEntityId, const char *targetComponentTypeName, ShovelerComponent *sourceComponent, void *unused);
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
	view->dependencies = g_hash_table_new_full(qualifiedComponentHash, qualifiedComponentEqual, freeQualifiedComponent, freeQualifiedComponents);
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

ShovelerComponentType *shovelerViewGetComponentType(ShovelerView *view, const char *componentTypeName)
{
	return g_hash_table_lookup(view->componentTypes, componentTypeName);
}

void shovelerViewAddDependency(ShovelerView *view, long long int sourceEntityId, const char *sourceComponentTypeName, long long int targetEntityId, const char *targetComponentTypeName)
{
	ShovelerViewQualifiedComponent *dependencySource = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencySource->entityId = sourceEntityId;
	dependencySource->componentTypeName = strdup(sourceComponentTypeName);

	ShovelerViewQualifiedComponent *dependencyTarget = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencyTarget->entityId = targetEntityId;
	dependencyTarget->componentTypeName = strdup(targetComponentTypeName);

	GQueue *dependencies = g_hash_table_lookup(view->dependencies, dependencySource);
	if(dependencies == NULL) {
		ShovelerViewQualifiedComponent *dependenciesKey = copyQualifiedComponent(dependencySource);
		dependencies = g_queue_new();
		g_hash_table_insert(view->dependencies, dependenciesKey, dependencies);
	}

	GQueue *reverseDependencies = g_hash_table_lookup(view->reverseDependencies, dependencyTarget);
	if(reverseDependencies == NULL) {
		ShovelerViewQualifiedComponent *reverseDependenciesKey = copyQualifiedComponent(dependencyTarget);
		reverseDependencies = g_queue_new();
		g_hash_table_insert(view->reverseDependencies, reverseDependenciesKey, reverseDependencies);
	}

	g_queue_push_tail(dependencies, dependencyTarget);
	g_queue_push_tail(reverseDependencies, dependencySource);

	view->numComponentDependencies++;
	shovelerLogTrace("Added dependency from component '%s' of entity %lld to component '%s' of entity %lld.", sourceComponentTypeName, sourceEntityId, targetComponentTypeName, targetEntityId);

	ShovelerViewEntity *sourceEntity = g_hash_table_lookup(view->entities, &sourceEntityId);
	if(sourceEntity != NULL) {
		ShovelerComponent *sourceComponent = g_hash_table_lookup(sourceEntity->components, sourceComponentTypeName);
		if(sourceComponent != NULL && shovelerComponentIsActive(sourceComponent)) {
			if(!checkDependency(view, dependencyTarget)) {
				// Since the source component exists but the target component either doesn't
				// exist or isn't active, we need to deactivate the source.
				shovelerComponentDeactivate(sourceComponent);
			}
		}
	}

	for(GList *iter = view->dependencyCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerViewDependencyCallback *callback = iter->data;
		callback->function(view, dependencySource, dependencyTarget, true, callback->userData);
	}
}

bool shovelerViewRemoveDependency(ShovelerView *view, long long int sourceEntityId, const char *sourceComponentTypeName, long long int targetEntityId, const char *targetComponentTypeName)
{
	ShovelerViewQualifiedComponent dependencySource;
	dependencySource.entityId = sourceEntityId;
	dependencySource.componentTypeName = (char *) sourceComponentTypeName; // won't be modified

	ShovelerViewQualifiedComponent dependencyTarget;
	dependencyTarget.entityId = targetEntityId;
	dependencyTarget.componentTypeName = (char *) targetComponentTypeName; // won't be modified

	GQueue *dependencies = g_hash_table_lookup(view->dependencies, &dependencySource);
	if(dependencies == NULL) {
		return false;
	}

	GQueue *reverseDependencies = g_hash_table_lookup(view->reverseDependencies, &dependencyTarget);
	assert(reverseDependencies != NULL);

	if(!removeDependency(dependencies, &dependencyTarget)) {
		return false;
	}

	bool reverseDependencyRemoved = removeDependency(reverseDependencies, &dependencySource);
	assert(reverseDependencyRemoved);

	view->numComponentDependencies--;
	shovelerLogTrace("Removed dependency from component '%s' of entity %lld to component '%s' of entity %lld.", sourceComponentTypeName, sourceEntityId, targetComponentTypeName, targetEntityId);

	for(GList *iter = view->dependencyCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerViewDependencyCallback *callback = iter->data;
		callback->function(view, &dependencySource, &dependencyTarget, false, callback->userData);
	}

	return true;
}

bool shovelerViewCheckDependencies(ShovelerView *view, long long int sourceEntityId, const char *sourceComponentTypeName)
{
	ShovelerViewQualifiedComponent dependencySource;
	dependencySource.entityId = sourceEntityId;
	dependencySource.componentTypeName = (char *) sourceComponentTypeName; // won't be modified

	GQueue *dependencies = g_hash_table_lookup(view->dependencies, &dependencySource);
	if(dependencies == NULL) {
		return true;
	}

	for(GList *iter = dependencies->head; iter != NULL; iter = iter->next) {
		const ShovelerViewQualifiedComponent *dependencyTarget = iter->data;

		if(!checkDependency(view, dependencyTarget)) {
			return false;
		}
	}

	return true;
}

void shovelerViewForEachReverseDependency(ShovelerView *view, long long int targetEntityId, const char *targetComponentTypeName, ShovelerViewForEachReverseDependencyCallbackFunction *callbackFunction, void *userData)
{
	ShovelerViewQualifiedComponent dependencyTarget;
	dependencyTarget.entityId = targetEntityId;
	dependencyTarget.componentTypeName = (char *) targetComponentTypeName; // won't be modified

	GQueue *reverseDependencies = g_hash_table_lookup(view->reverseDependencies, &dependencyTarget);
	if(reverseDependencies != NULL) {
		for(GList *iter = reverseDependencies->head; iter != NULL; iter = iter->next) {
			const ShovelerViewQualifiedComponent *dependencySource = iter->data;

			ShovelerViewEntity *sourceEntity = g_hash_table_lookup(view->entities, &dependencySource->entityId);
			if(sourceEntity != NULL) {
				ShovelerComponent *sourceComponent = g_hash_table_lookup(sourceEntity->components, dependencySource->componentTypeName);
				if(sourceComponent != NULL) {
					callbackFunction(view, targetEntityId, targetComponentTypeName, sourceComponent, userData);
				}
			}
		}
	}
}

void shovelerViewActivateReverseDependencies(ShovelerView *view, long long int targetEntityId, const char *targetComponentTypeName)
{
	shovelerViewForEachReverseDependency(view, targetEntityId, targetComponentTypeName, activateReverseDependency, /* userData */ NULL);
}

void shovelerViewDeactivateReverseDependencies(ShovelerView *view, long long int targetEntityId, const char *targetComponentTypeName)
{
	shovelerViewForEachReverseDependency(view, targetEntityId, targetComponentTypeName, deactivateReverseDependency, /* userData */ NULL);
}

ShovelerViewEntity *shovelerViewAddEntity(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = malloc(sizeof(ShovelerViewEntity));
	entity->view = view;
	entity->entityId = entityId;
	entity->type = NULL;
	entity->components = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeComponent);
	entity->authoritativeComponents = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
	entity->callbacks = g_hash_table_new_full(g_str_hash, g_str_equal, free, freeCallbacks);

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

	GList *componentTypeNames = g_hash_table_get_keys(entity->components);
	for(GList *iter = componentTypeNames; iter != NULL; iter = iter->next) {
		char *componentTypeName = iter->data;
		shovelerViewEntityRemoveComponent(entity, componentTypeName);
	}
	g_list_free(componentTypeNames);

	assert(g_hash_table_size(entity->components) == 0);

	if(!g_hash_table_remove(view->entities, &entityId)) {
		return false;
	}

	view->numEntities--;
	shovelerLogTrace("Removed entity %lld.", entityId);
	return true;
}

void shovelerViewEntityDelegate(ShovelerViewEntity *entity, const char *componentTypeName)
{
	char *copiedComponentTypeName = strdup(componentTypeName);
	g_hash_table_add(entity->authoritativeComponents, copiedComponentTypeName);
}

bool shovelerViewEntityIsAuthoritative(ShovelerViewEntity *entity, const char *componentTypeName)
{
	return g_hash_table_contains(entity->authoritativeComponents, componentTypeName);
}

void shovelerViewEntityUndelegate(ShovelerViewEntity *entity, const char *componentTypeName)
{
	g_hash_table_remove(entity->authoritativeComponents, componentTypeName);

	ShovelerComponent *component = g_hash_table_lookup(entity->components, componentTypeName);
	if(component != NULL && component->type->requiresAuthority) {
		shovelerComponentDeactivate(component);
	}
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

ShovelerComponent *shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentTypeName)
{
	ShovelerComponent *component = shovelerComponentCreate(entity, componentTypeName);
	if(component == NULL) {
		return NULL;
	}

	if(!g_hash_table_insert(entity->components, component->type->name, component)) {
		shovelerComponentFree(component);
		return NULL;
	}

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_ADD);

	entity->view->numComponents++;
	shovelerLogTrace("Added component '%s' to entity %lld.", componentTypeName, entity->entityId);
	return component;
}

bool shovelerViewEntityRemoveComponent(ShovelerViewEntity *entity, const char *componentTypeName)
{
	ShovelerComponent *component = g_hash_table_lookup(entity->components, componentTypeName);
	if(component == NULL) {
		return false;
	}

	triggerComponentCallback(component, SHOVELER_VIEW_COMPONENT_CALLBACK_REMOVE);

	component->entity->view->numComponents--;
	shovelerLogTrace("Removed component '%s' from entity %lld.", componentTypeName, entity->entityId);

	g_hash_table_remove(entity->components, componentTypeName);
	return true;
}

ShovelerViewComponentCallback *shovelerViewEntityAddCallback(ShovelerViewEntity *entity, const char *componentTypeName, ShovelerViewComponentCallbackFunction *function, void *userData)
{
	GQueue *callbacks = g_hash_table_lookup(entity->callbacks, componentTypeName);
	if(callbacks == NULL) {
		callbacks = g_queue_new();
		g_hash_table_insert(entity->callbacks, strdup(componentTypeName), callbacks);
	}

	ShovelerViewComponentCallback *callback = malloc(sizeof(ShovelerViewComponentCallback));
	callback->function = function;
	callback->userData = userData;
	g_queue_push_tail(callbacks, callback);

	// if the target component is already there, trigger the add callback directly
	ShovelerComponent *component = g_hash_table_lookup(entity->components, componentTypeName);
	if(component != NULL) {
		callback->function(component, SHOVELER_VIEW_COMPONENT_CALLBACK_ADD, callback->userData);
	}

	return callback;
}

bool shovelerViewSetTarget(ShovelerView *view, const char *targetName, void *target)
{
	return g_hash_table_insert(view->targets, strdup(targetName), target);
}

bool shovelerViewEntityRemoveCallback(ShovelerViewEntity *entity, const char *componentTypeName, ShovelerViewComponentCallback *callback)
{
	GQueue *callbacks = g_hash_table_lookup(entity->callbacks, componentTypeName);
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
		const char *componentTypeName;
		ShovelerComponent *component;
		g_hash_table_iter_init(&componentIter, entity->components);
		while(g_hash_table_iter_next(&componentIter, (gpointer *) &componentTypeName, (gpointer *) &component)) {
			const char *color = shovelerComponentIsActive(component) ? "green" : "red";
			g_string_append_printf(graph, "		entity%lld_%s [label = <<font color='%s'>%s</font>>];\n", entityId, componentTypeName, color, componentTypeName);
		}

		g_string_append(graph, "	}\n");

		g_hash_table_iter_init(&componentIter, entity->components);
		while(g_hash_table_iter_next(&componentIter, (gpointer *) &componentTypeName, (gpointer *) &component)) {
			ShovelerViewQualifiedComponent dependencySource;
			dependencySource.entityId = entity->entityId;
			dependencySource.componentTypeName = (char *) componentTypeName; // won't be modified

			GQueue *dependencies = g_hash_table_lookup(view->dependencies, &dependencySource);
			if(dependencies == NULL) {
				for(GList *dependencyIter = dependencies->head; dependencyIter != NULL; dependencyIter = dependencyIter->next) {
					const ShovelerViewQualifiedComponent *dependencyTarget = (ShovelerViewQualifiedComponent *) dependencyIter->data;
					g_string_append_printf(graph, "	entity%lld_%s -> entity%lld_%s;\n", entityId, componentTypeName, dependencyTarget->entityId, dependencyTarget->componentTypeName);
				}
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
	g_hash_table_destroy(view->entities);
	g_queue_free_full(view->dependencyCallbacks, free);
	g_hash_table_destroy(view->targets);
	g_hash_table_destroy(view->reverseDependencies);
	g_hash_table_destroy(view->dependencies);
	g_hash_table_destroy(view->componentTypes);
	free(view);
}

guint qualifiedComponentHash(gconstpointer qualifiedComponentPointer)
{
	const ShovelerViewQualifiedComponent *qualifiedComponent = (ShovelerViewQualifiedComponent *) qualifiedComponentPointer;

	guint entityIdHash = g_int64_hash(&qualifiedComponent->entityId);
	guint componentTypeNameHash = g_str_hash(qualifiedComponent->componentTypeName);

	return shovelerHashCombine(entityIdHash, componentTypeNameHash);
}

gboolean qualifiedComponentEqual(gconstpointer firstQualifiedComponentPointer, gconstpointer secondQualifiedComponentPointer)
{
	const ShovelerViewQualifiedComponent *firstQualifiedComponent = (ShovelerViewQualifiedComponent *) firstQualifiedComponentPointer;
	const ShovelerViewQualifiedComponent *secondQualifiedComponent = (ShovelerViewQualifiedComponent *) secondQualifiedComponentPointer;

	return firstQualifiedComponent->entityId == secondQualifiedComponent->entityId
		&& strcmp(firstQualifiedComponent->componentTypeName, secondQualifiedComponent->componentTypeName) == 0;
}

static void triggerComponentCallback(ShovelerComponent *component, ShovelerViewComponentCallbackType callbackType)
{
	GQueue *callbacks = g_hash_table_lookup(component->entity->callbacks, component->type->name);
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
	copiedQualifiedComponent->componentTypeName = strdup(qualifiedComponent->componentTypeName);
	return copiedQualifiedComponent;
}

static bool checkDependency(ShovelerView *view, const ShovelerViewQualifiedComponent *dependencyTarget)
{
	ShovelerViewEntity *targetEntity = g_hash_table_lookup(view->entities, &dependencyTarget->entityId);
	if(targetEntity != NULL) {
		ShovelerComponent *targetComponent = g_hash_table_lookup(targetEntity->components, dependencyTarget->componentTypeName);
		if(targetComponent != NULL && shovelerComponentIsActive(targetComponent)) {
			return true;
		}
	}

	return false;
}

static bool removeDependency(GQueue *dependencies, const ShovelerViewQualifiedComponent *dependency)
{
	for(GList *iter = dependencies->head; iter != NULL; iter = iter->next) {
		ShovelerViewQualifiedComponent *currentDependency = iter->data;
		if(currentDependency->entityId == dependency->entityId
			&& strcmp(currentDependency->componentTypeName, dependency->componentTypeName) == 0) {
			g_queue_delete_link(dependencies, iter);
			freeQualifiedComponent(currentDependency);
			return true;
		}
	}

	return false;
}

static void activateReverseDependency(ShovelerView *view, long long int targetEntityId, const char *targetComponentTypeName, ShovelerComponent *sourceComponent, void *unused)
{
	shovelerComponentActivate(sourceComponent);
}

static void deactivateReverseDependency(ShovelerView *view, long long int targetEntityId, const char *targetComponentTypeName, ShovelerComponent *sourceComponent, void *unused)
{
	shovelerComponentDeactivate(sourceComponent);
}

static void freeComponentType(void *componentTypePointer)
{
	ShovelerComponentType *componentType = componentTypePointer;
	shovelerComponentTypeFree(componentType);
}

static void freeEntity(void *entityPointer)
{
	ShovelerViewEntity *entity = entityPointer;

	g_hash_table_destroy(entity->callbacks);
	g_hash_table_destroy(entity->authoritativeComponents);
	g_hash_table_destroy(entity->components);
	free(entity->type);
	free(entity);
}

static void freeComponent(void *componentPointer)
{
	ShovelerComponent *component = componentPointer;
	shovelerComponentFree(component);
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
	free(qualifiedComponent->componentTypeName);
	free(qualifiedComponent);
}
