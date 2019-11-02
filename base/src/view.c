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
static ShovelerViewQualifiedComponent *copyQualifiedComponent(ShovelerViewQualifiedComponent *qualifiedComponent);
static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentTypeId, void *viewPointer);
static void *getTarget(ShovelerComponent *component, const char *targetName, void *viewPointer);
static void addComponentDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *viewPointer);
static bool removeComponentDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *viewPointer);
static void forEachComponentReverseDependency(ShovelerComponent *targetComponent, ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *viewPointer);
static void reportComponentActivation(ShovelerComponent *component, int delta, void *viewPointer);
static bool removeDependency(GQueue *dependencies, const ShovelerViewQualifiedComponent *dependency);
static void freeComponentType(void *componentTypePointer);
static void freeEntity(void *entityPointer);
static void freeComponent(void *componentPointer);
static void freeQualifiedComponents(void *qualifiedComponentsPointer);

ShovelerView *shovelerViewCreate()
{
	ShovelerView *view = malloc(sizeof(ShovelerView));
	view->componentTypes = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeComponentType);
	view->entities = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, freeEntity);
	view->targets = g_hash_table_new_full(&g_str_hash, &g_str_equal, &free, NULL);
	view->dependencies = g_hash_table_new_full(qualifiedComponentHash, qualifiedComponentEqual, free, freeQualifiedComponents);
	view->reverseDependencies = g_hash_table_new_full(qualifiedComponentHash, qualifiedComponentEqual, free, freeQualifiedComponents);
	view->adapter = malloc(sizeof(ShovelerComponentViewAdapter));
	view->adapter->getComponent = getComponent;
	view->adapter->getTarget = getTarget;
	view->adapter->addDependency = addComponentDependency;
	view->adapter->removeDependency = removeComponentDependency;
	view->adapter->forEachReverseDependency = forEachComponentReverseDependency;
	view->adapter->reportActivation = reportComponentActivation;
	view->adapter->userData = view;
	view->numEntities = 0;
	view->numComponents = 0;
	view->numComponentDependencies = 0;
	view->numActiveComponents = 0;
	view->numDelegatedComponents = 0;

	return view;
}

bool shovelerViewAddComponentType(ShovelerView *view, ShovelerComponentType *componentType)
{
	return g_hash_table_insert(view->componentTypes, (gpointer) componentType->id, componentType);
}

ShovelerComponentType *shovelerViewGetComponentType(ShovelerView *view, const char *componentTypeId)
{
	return g_hash_table_lookup(view->componentTypes, componentTypeId);
}

void shovelerViewAddDependency(ShovelerView *view, long long int sourceEntityId, const char *sourceComponentTypeId, long long int targetEntityId, const char *targetComponentTypeId)
{
	ShovelerViewQualifiedComponent *dependencySource = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencySource->entityId = sourceEntityId;
	dependencySource->componentTypeId = sourceComponentTypeId;

	ShovelerViewQualifiedComponent *dependencyTarget = malloc(sizeof(ShovelerViewQualifiedComponent));
	dependencyTarget->entityId = targetEntityId;
	dependencyTarget->componentTypeId = targetComponentTypeId;

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
	shovelerLogTrace("Added dependency from component '%s' of entity %lld to component '%s' of entity %lld.", sourceComponentTypeId, sourceEntityId, targetComponentTypeId, targetEntityId);
}

bool shovelerViewRemoveDependency(ShovelerView *view, long long int sourceEntityId, const char *sourceComponentTypeId, long long int targetEntityId, const char *targetComponentTypeId)
{
	ShovelerViewQualifiedComponent dependencySource;
	dependencySource.entityId = sourceEntityId;
	dependencySource.componentTypeId = sourceComponentTypeId;

	ShovelerViewQualifiedComponent dependencyTarget;
	dependencyTarget.entityId = targetEntityId;
	dependencyTarget.componentTypeId = targetComponentTypeId;

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
	shovelerLogTrace("Removed dependency from component '%s' of entity %lld to component '%s' of entity %lld.", sourceComponentTypeId, sourceEntityId, targetComponentTypeId, targetEntityId);

	return true;
}

ShovelerViewEntity *shovelerViewAddEntity(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = malloc(sizeof(ShovelerViewEntity));
	entity->view = view;
	entity->id = entityId;
	entity->type = NULL;
	entity->components = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeComponent);
	entity->authoritativeComponents = g_hash_table_new(g_direct_hash, g_direct_equal);

	if(!g_hash_table_insert(view->entities, &entity->id, entity)) {
		freeEntity(entity);
		return NULL;
	}

	view->numEntities++;
	shovelerLogTrace("Added entity %lld.", entity->id);
	return entity;
}

bool shovelerViewRemoveEntity(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = g_hash_table_lookup(view->entities, &entityId);
	if(entity == NULL) {
		return false;
	}

	GList *componentTypeIds = g_hash_table_get_keys(entity->components);
	for(GList *iter = componentTypeIds; iter != NULL; iter = iter->next) {
		char *componentTypeId = iter->data;
		shovelerViewEntityRemoveComponent(entity, componentTypeId);
	}
	g_list_free(componentTypeIds);

	assert(g_hash_table_size(entity->components) == 0);

	if(!g_hash_table_remove(view->entities, &entityId)) {
		return false;
	}

	view->numEntities--;
	shovelerLogTrace("Removed entity %lld.", entityId);
	return true;
}

void shovelerViewEntityDelegate(ShovelerViewEntity *entity, const char *componentTypeId)
{
	g_hash_table_add(entity->authoritativeComponents, (gpointer) componentTypeId);

	ShovelerComponent *component = g_hash_table_lookup(entity->components, componentTypeId);
	if(component != NULL) {
		shovelerComponentDelegate(component);
	}
}

bool shovelerViewEntityIsAuthoritative(ShovelerViewEntity *entity, const char *componentTypeId)
{
	return g_hash_table_contains(entity->authoritativeComponents, componentTypeId);
}

void shovelerViewEntityUndelegate(ShovelerViewEntity *entity, const char *componentTypeId)
{
	g_hash_table_remove(entity->authoritativeComponents, componentTypeId);

	ShovelerComponent *component = g_hash_table_lookup(entity->components, componentTypeId);
	if(component != NULL) {
		shovelerComponentUndelegate(component);
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

ShovelerComponent *shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentTypeId)
{
	ShovelerComponentType *componentType = g_hash_table_lookup(entity->view->componentTypes, componentTypeId);
	if(componentType == NULL) {
		return NULL;
	}

	ShovelerComponent *component = shovelerComponentCreate(entity->view->adapter, entity->id, componentType);
	if(component == NULL) {
		return NULL;
	}

	if(!g_hash_table_insert(entity->components, (gpointer) component->type->id, component)) {
		shovelerComponentFree(component);
		return NULL;
	}

	entity->view->numComponents++;
	shovelerLogTrace("Added component '%s' to entity %lld.", componentTypeId, entity->id);
	return component;
}

bool shovelerViewEntityRemoveComponent(ShovelerViewEntity *entity, const char *componentTypeId)
{
	ShovelerComponent *component = g_hash_table_lookup(entity->components, componentTypeId);
	if(component == NULL) {
		return false;
	}

	entity->view->numComponents--;
	shovelerLogTrace("Removed component '%s' from entity %lld.", componentTypeId, entity->id);

	g_hash_table_remove(entity->components, componentTypeId);
	return true;
}

bool shovelerViewSetTarget(ShovelerView *view, const char *targetName, void *target)
{
	return g_hash_table_insert(view->targets, strdup(targetName), target);
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
		const char *componentTypeId;
		ShovelerComponent *component;
		g_hash_table_iter_init(&componentIter, entity->components);
		while(g_hash_table_iter_next(&componentIter, (gpointer *) &componentTypeId, (gpointer *) &component)) {
			const char *color = shovelerComponentIsActive(component) ? "green" : "red";
			g_string_append_printf(graph, "		entity%lld_%s [label = <<font color='%s'>%s</font>>];\n", entityId, componentTypeId, color, componentTypeId);
		}

		g_string_append(graph, "	}\n");

		g_hash_table_iter_init(&componentIter, entity->components);
		while(g_hash_table_iter_next(&componentIter, (gpointer *) &componentTypeId, (gpointer *) &component)) {
			ShovelerViewQualifiedComponent dependencySource;
			dependencySource.entityId = entity->id;
			dependencySource.componentTypeId = (char *) componentTypeId; // won't be modified

			GQueue *dependencies = g_hash_table_lookup(view->dependencies, &dependencySource);
			if(dependencies != NULL) {
				for(GList *dependencyIter = dependencies->head; dependencyIter != NULL; dependencyIter = dependencyIter->next) {
					const ShovelerViewQualifiedComponent *dependencyTarget = (ShovelerViewQualifiedComponent *) dependencyIter->data;
					g_string_append_printf(graph, "	entity%lld_%s -> entity%lld_%s;\n", entityId, componentTypeId, dependencyTarget->entityId, dependencyTarget->componentTypeId);
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

void shovelerViewFree(ShovelerView *view)
{
	g_hash_table_destroy(view->entities);
	g_hash_table_destroy(view->targets);
	g_hash_table_destroy(view->reverseDependencies);
	g_hash_table_destroy(view->dependencies);
	g_hash_table_destroy(view->componentTypes);
	free(view->adapter);
	free(view);
}

guint qualifiedComponentHash(gconstpointer qualifiedComponentPointer)
{
	const ShovelerViewQualifiedComponent *qualifiedComponent = (const ShovelerViewQualifiedComponent *) qualifiedComponentPointer;

	guint entityIdHash = g_int64_hash(&qualifiedComponent->entityId);
	guint componentTypeIdHash = g_str_hash(qualifiedComponent->componentTypeId);

	return shovelerHashCombine(entityIdHash, componentTypeIdHash);
}

gboolean qualifiedComponentEqual(gconstpointer firstQualifiedComponentPointer, gconstpointer secondQualifiedComponentPointer)
{
	const ShovelerViewQualifiedComponent *firstQualifiedComponent = (const ShovelerViewQualifiedComponent *) firstQualifiedComponentPointer;
	const ShovelerViewQualifiedComponent *secondQualifiedComponent = (const ShovelerViewQualifiedComponent *) secondQualifiedComponentPointer;

	return firstQualifiedComponent->entityId == secondQualifiedComponent->entityId
		&& firstQualifiedComponent->componentTypeId == secondQualifiedComponent->componentTypeId;
}

static ShovelerViewQualifiedComponent *copyQualifiedComponent(ShovelerViewQualifiedComponent *qualifiedComponent)
{
	ShovelerViewQualifiedComponent *copiedQualifiedComponent = malloc(sizeof(ShovelerViewQualifiedComponent));
	copiedQualifiedComponent->entityId = qualifiedComponent->entityId;
	copiedQualifiedComponent->componentTypeId = qualifiedComponent->componentTypeId;
	return copiedQualifiedComponent;
}

static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentTypeId, void *viewPointer)
{
	ShovelerView *view = (ShovelerView *) viewPointer;

	ShovelerViewEntity *entity = g_hash_table_lookup(view->entities, &entityId);
	if(entity == NULL) {
		return NULL;
	}

	return g_hash_table_lookup(entity->components, componentTypeId);
}

static void *getTarget(ShovelerComponent *component, const char *targetName, void *viewPointer)
{
	ShovelerView *view = (ShovelerView *) viewPointer;

	return g_hash_table_lookup(view->targets, targetName);
}

static void addComponentDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *viewPointer)
{
	ShovelerView *view = (ShovelerView *) viewPointer;

	shovelerViewAddDependency(view, component->entityId, component->type->id, targetEntityId, targetComponentTypeId);
}

static bool removeComponentDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *viewPointer)
{
	ShovelerView *view = (ShovelerView *) viewPointer;

	return shovelerViewRemoveDependency(view, component->entityId, component->type->id, targetEntityId, targetComponentTypeId);
}

static void forEachComponentReverseDependency(ShovelerComponent *targetComponent, ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *viewPointer) {
	ShovelerView *view = (ShovelerView *) viewPointer;

	ShovelerViewQualifiedComponent dependencyTarget;
	dependencyTarget.entityId = targetComponent->entityId;
	dependencyTarget.componentTypeId = targetComponent->type->id;

	GQueue *reverseDependencies = g_hash_table_lookup(view->reverseDependencies, &dependencyTarget);
	if(reverseDependencies != NULL) {
		for(GList *iter = reverseDependencies->head; iter != NULL; iter = iter->next) {
			const ShovelerViewQualifiedComponent *dependencySource = iter->data;

			ShovelerViewEntity *sourceEntity = g_hash_table_lookup(view->entities, &dependencySource->entityId);
			if(sourceEntity != NULL) {
				ShovelerComponent *sourceComponent = g_hash_table_lookup(sourceEntity->components, dependencySource->componentTypeId);
				if(sourceComponent != NULL) {
					callbackFunction(sourceComponent, targetComponent, callbackUserData);
				}
			}
		}
	}
}

static void reportComponentActivation(ShovelerComponent *component, int delta, void *viewPointer)
{
	ShovelerView *view = (ShovelerView *) viewPointer;

	view->numActiveComponents += delta;
}

static bool removeDependency(GQueue *dependencies, const ShovelerViewQualifiedComponent *dependency)
{
	for(GList *iter = dependencies->head; iter != NULL; iter = iter->next) {
		ShovelerViewQualifiedComponent *currentDependency = iter->data;
		if(currentDependency->entityId == dependency->entityId
			&& strcmp(currentDependency->componentTypeId, dependency->componentTypeId) == 0) {
			g_queue_delete_link(dependencies, iter);
			free(currentDependency);
			return true;
		}
	}

	return false;
}

static void freeComponentType(void *componentTypePointer)
{
	ShovelerComponentType *componentType = componentTypePointer;
	shovelerComponentTypeFree(componentType);
}

static void freeEntity(void *entityPointer)
{
	ShovelerViewEntity *entity = entityPointer;

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
	g_queue_free_full(qualifiedComponents, free);
}
