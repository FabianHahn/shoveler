#include "shoveler/world.h"

#include <glib.h>
#include <stdlib.h> // malloc free
#include <string.h> // memset

#include "shoveler/component.h"
#include "shoveler/component_system.h"
#include "shoveler/component_type.h"
#include "shoveler/entity_component_id.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static ShovelerComponent* getComponent(
    ShovelerComponent* component,
    long long int entityId,
    const char* componentTypeId,
    void* userData);
static void worldUpdateComponent(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* worldPointer);
static void worldActivateComponent(ShovelerComponent* component, void* worldPointer);
static void worldDeactivateComponent(ShovelerComponent* component, void* worldPointer);
static void addDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* userData);
static bool removeDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* userData);
static void forEachReverseDependency(
    ShovelerComponent* component,
    ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction* callbackFunction,
    void* callbackUserData,
    void* adapterUserData);
static bool removeDependencyListEntry(
    GArray* dependencyList, const ShovelerEntityComponentId* entry);
static void freeEntity(void* entityPointer);
static void freeComponent(void* componentPointer);
static void freeDependencyArray(void* dependencyArrayPointer);

ShovelerWorldCallbacks shovelerWorldCallbacks() {
  ShovelerWorldCallbacks callbacks;
  memset(&callbacks, 0, sizeof(ShovelerWorldCallbacks));
  return callbacks;
}

ShovelerWorld* shovelerWorldCreate(
    ShovelerSchema* schema, ShovelerSystem* system, ShovelerWorldCallbacks* callbacks) {
  ShovelerWorld* world = malloc(sizeof(ShovelerWorld));
  world->entities = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, freeEntity);
  world->dependencies = g_hash_table_new_full(
      shovelerEntityComponentIdHash, shovelerEntityComponentIdEqual, free, freeDependencyArray);
  world->reverseDependencies = g_hash_table_new_full(
      shovelerEntityComponentIdHash, shovelerEntityComponentIdEqual, free, freeDependencyArray);
  world->schema = schema;
  world->system = system;
  world->callbacks = callbacks;
  world->componentWorldAdapter = malloc(sizeof(ShovelerComponentWorldAdapter));
  world->componentWorldAdapter->getComponent = getComponent;
  world->componentWorldAdapter->onUpdateComponentField = worldUpdateComponent;
  world->componentWorldAdapter->onActivateComponent = worldActivateComponent;
  world->componentWorldAdapter->onDeactivateComponent = worldDeactivateComponent;
  world->componentWorldAdapter->addDependency = addDependency;
  world->componentWorldAdapter->removeDependency = removeDependency;
  world->componentWorldAdapter->forEachReverseDependency = forEachReverseDependency;
  world->componentWorldAdapter->userData = world;
  world->numComponentDependencies = 0;
  world->numComponents = 0;

  return world;
}

ShovelerComponentWorldAdapter* shovelerWorldGetComponentAdapter(ShovelerWorld* world) {
  return world->componentWorldAdapter;
}

ShovelerWorldEntity* shovelerWorldAddEntity(ShovelerWorld* world, long long int entityId) {
  ShovelerWorldEntity* entity = malloc(sizeof(ShovelerWorldEntity));
  entity->world = world;
  entity->id = entityId;
  entity->label = NULL;
  entity->components = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeComponent);
  entity->authoritativeComponents = g_hash_table_new(g_direct_hash, g_direct_equal);

  if (!g_hash_table_insert(world->entities, &entity->id, entity)) {
    freeEntity(entity);
    return NULL;
  }

  shovelerLogTrace("Added entity %lld.", entity->id);

  if (world->callbacks->onAddEntity != NULL) {
    world->callbacks->onAddEntity(world, entity, world->callbacks->userData);
  }

  return entity;
}

bool shovelerWorldRemoveEntity(ShovelerWorld* world, long long int entityId) {
  ShovelerWorldEntity* entity = g_hash_table_lookup(world->entities, &entityId);
  if (entity == NULL) {
    return false;
  }

  GList* componentTypeIds = g_hash_table_get_keys(entity->components);
  for (GList* iter = componentTypeIds; iter != NULL; iter = iter->next) {
    char* componentTypeId = iter->data;
    shovelerWorldEntityRemoveComponent(entity, componentTypeId);
  }
  g_list_free(componentTypeIds);

  assert(g_hash_table_size(entity->components) == 0);

  if (!g_hash_table_remove(world->entities, &entityId)) {
    return false;
  }

  shovelerLogTrace("Removed entity %lld.", entityId);

  if (world->callbacks->onRemoveEntity != NULL) {
    world->callbacks->onRemoveEntity(world, entityId, world->callbacks->userData);
  }

  return true;
}

ShovelerComponent* shovelerWorldEntityAddComponent(
    ShovelerWorldEntity* entity, const char* componentTypeId) {
  ShovelerWorld* world = entity->world;

  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(world->schema, componentTypeId);
  if (componentType == NULL) {
    shovelerLogWarning(
        "Tried to add component with unknown type '%s' that is not present in schema to entity "
        "%lld, ignoring.",
        componentTypeId,
        entity->id);
    return NULL;
  }

  ShovelerComponent* component =
      g_hash_table_lookup(entity->components, (gpointer) componentTypeId);
  if (component != NULL) {
    shovelerLogWarning(
        "Tried to already existing component '%s' to entity %lld, ignoring.",
        componentTypeId,
        entity->id);
    return NULL;
  }

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(world->system, componentType);
  component = shovelerComponentCreate(
      world->componentWorldAdapter, componentSystem->componentAdapter, entity->id, componentType);

  if (!g_hash_table_insert(entity->components, (gpointer) component->type->id, component)) {
    shovelerComponentFree(component);
    return NULL;
  }

  if (g_hash_table_lookup(entity->authoritativeComponents, (gpointer) componentTypeId) != NULL) {
    shovelerComponentDelegate(component);
  }

  world->numComponents++;
  shovelerLogTrace("Added component '%s' to entity %lld.", componentTypeId, entity->id);

  if (world->callbacks->onAddComponent != NULL) {
    world->callbacks->onAddComponent(world, entity, component, world->callbacks->userData);
  }

  return component;
}

bool shovelerWorldEntityRemoveComponent(ShovelerWorldEntity* entity, const char* componentTypeId) {
  ShovelerWorld* world = entity->world;

  ShovelerComponent* component = g_hash_table_lookup(entity->components, componentTypeId);
  if (component == NULL) {
    return false;
  }

  // Deactivate the component before removing it. This is important because we don't want reverse
  // dependencies to be deactivated within the hash map removal below, which would then make this
  // component no longer visible to them during reverse dependency deactivation.
  shovelerComponentDeactivate(component);
  g_hash_table_remove(entity->components, componentTypeId);

  world->numComponents--;
  shovelerLogTrace("Removed component '%s' from entity %lld.", componentTypeId, entity->id);

  if (world->callbacks->onRemoveComponent != NULL) {
    world->callbacks->onRemoveComponent(world, entity, componentTypeId, world->callbacks->userData);
  }

  return true;
}

void shovelerWorldEntityDelegateComponent(
    ShovelerWorldEntity* entity, const char* componentTypeId) {
  g_hash_table_add(entity->authoritativeComponents, (gpointer) componentTypeId);

  ShovelerComponent* component = g_hash_table_lookup(entity->components, componentTypeId);
  if (component != NULL) {
    shovelerComponentDelegate(component);
  }

  if (entity->world->callbacks->onDelegateComponent != NULL) {
    entity->world->callbacks->onDelegateComponent(
        entity->world, entity, componentTypeId, entity->world->callbacks->userData);
  }
}

bool shovelerWorldEntityIsAuthoritative(ShovelerWorldEntity* entity, const char* componentTypeId) {
  return g_hash_table_contains(entity->authoritativeComponents, componentTypeId);
}

void shovelerWorldEntityUndelegateComponent(
    ShovelerWorldEntity* entity, const char* componentTypeId) {
  g_hash_table_remove(entity->authoritativeComponents, componentTypeId);

  ShovelerComponent* component = g_hash_table_lookup(entity->components, componentTypeId);
  if (component != NULL) {
    shovelerComponentUndelegate(component);
  }

  if (entity->world->callbacks->onUndelegateComponent != NULL) {
    entity->world->callbacks->onUndelegateComponent(
        entity->world, entity, componentTypeId, entity->world->callbacks->userData);
  }
}

void shovelerWorldFree(ShovelerWorld* world) {
  // Get keys list because the keys set will be modified while we iterate over the list.
  GList* entityIdsList = g_hash_table_get_keys(world->entities);
  for (GList* iter = entityIdsList; iter != NULL; iter = iter->next) {
    long long int* entityIdPointer = iter->data;
    shovelerWorldRemoveEntity(world, *entityIdPointer);
  }
  g_list_free(entityIdsList);

  assert(g_hash_table_size(world->entities) == 0);
  assert(g_hash_table_size(world->reverseDependencies) == 0);
  assert(g_hash_table_size(world->dependencies) == 0);
  g_hash_table_destroy(world->entities);
  g_hash_table_destroy(world->reverseDependencies);
  g_hash_table_destroy(world->dependencies);
  free(world->componentWorldAdapter);
  free(world);
}

static ShovelerComponent* getComponent(
    ShovelerComponent* component,
    long long int entityId,
    const char* componentTypeId,
    void* worldPointer) {
  ShovelerWorld* world = (ShovelerWorld*) worldPointer;

  ShovelerWorldEntity* entity = g_hash_table_lookup(world->entities, &entityId);
  if (entity == NULL) {
    return NULL;
  }

  return g_hash_table_lookup(entity->components, componentTypeId);
}

static void worldUpdateComponent(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* worldPointer) {
  ShovelerWorld* world = (ShovelerWorld*) worldPointer;

  if (world->callbacks->onUpdateComponent != NULL) {
    ShovelerWorldEntity* entity = shovelerWorldGetEntity(world, component->entityId);
    assert(entity != NULL);
    world->callbacks->onUpdateComponent(
        world,
        entity,
        component,
        fieldId,
        field,
        value,
        isAuthoritative,
        world->callbacks->userData);
  }
}

static void worldActivateComponent(ShovelerComponent* component, void* worldPointer) {
  ShovelerWorld* world = (ShovelerWorld*) worldPointer;

  if (world->callbacks->onActivateComponent != NULL) {
    ShovelerWorldEntity* entity = shovelerWorldGetEntity(world, component->entityId);
    assert(entity != NULL);
    world->callbacks->onActivateComponent(world, entity, component, world->callbacks->userData);
  }
}

static void worldDeactivateComponent(ShovelerComponent* component, void* worldPointer) {
  ShovelerWorld* world = (ShovelerWorld*) worldPointer;

  if (world->callbacks->onDeactivateComponent != NULL) {
    ShovelerWorldEntity* entity = shovelerWorldGetEntity(world, component->entityId);
    assert(entity != NULL);
    world->callbacks->onDeactivateComponent(world, entity, component, world->callbacks->userData);
  }
}

static void addDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* worldPointer) {
  ShovelerWorld* world = (ShovelerWorld*) worldPointer;

  ShovelerEntityComponentId dependencySource =
      shovelerEntityComponentId(component->entityId, component->type->id);
  ShovelerEntityComponentId dependencyTarget =
      shovelerEntityComponentId(targetEntityId, targetComponentTypeId);

  GArray* dependencies = g_hash_table_lookup(world->dependencies, &dependencySource);
  if (dependencies == NULL) {
    ShovelerEntityComponentId* dependenciesKey = shovelerEntityComponentIdCopy(&dependencySource);
    dependencies = g_array_new(
        /* zeroTerminated */ false, /* clear */ true, sizeof(ShovelerEntityComponentId));
    g_hash_table_insert(world->dependencies, dependenciesKey, dependencies);
  }

  GArray* reverseDependencies = g_hash_table_lookup(world->reverseDependencies, &dependencyTarget);
  if (reverseDependencies == NULL) {
    ShovelerEntityComponentId* reverseDependenciesKey =
        shovelerEntityComponentIdCopy(&dependencyTarget);
    reverseDependencies = g_array_new(
        /* zeroTerminated */ false, /* clear */ true, sizeof(ShovelerEntityComponentId));
    g_hash_table_insert(world->reverseDependencies, reverseDependenciesKey, reverseDependencies);
  }

  g_array_append_val(dependencies, dependencyTarget);
  g_array_append_val(reverseDependencies, dependencySource);

  world->numComponentDependencies++;

  // Only print dependencies to other entities. Otherwise, we print all dummy dependencies added by
  // newly created components which makes for spammy and misleading logs.
  if (component->entityId != targetEntityId) {
    shovelerLogTrace(
        "Added dependency from component '%s' of entity %lld to component '%s' of entity %lld.",
        component->type->id,
        component->entityId,
        targetComponentTypeId,
        targetEntityId);
  }

  if (world->callbacks->onAddDependency != NULL) {
    world->callbacks->onAddDependency(
        world, &dependencySource, &dependencyTarget, world->callbacks->userData);
  }
}

static bool removeDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* worldPointer) {
  ShovelerWorld* world = (ShovelerWorld*) worldPointer;

  ShovelerEntityComponentId dependencySource =
      shovelerEntityComponentId(component->entityId, component->type->id);
  ShovelerEntityComponentId dependencyTarget =
      shovelerEntityComponentId(targetEntityId, targetComponentTypeId);

  GArray* dependencies = g_hash_table_lookup(world->dependencies, &dependencySource);
  if (dependencies == NULL) {
    return false;
  }

  GArray* reverseDependencies = g_hash_table_lookup(world->reverseDependencies, &dependencyTarget);
  assert(reverseDependencies != NULL);

  if (!removeDependencyListEntry(dependencies, &dependencyTarget)) {
    return false;
  }
  if (dependencies->len == 0) {
    // Clean up list if it is now empty.
    g_hash_table_remove(world->dependencies, &dependencySource);
  }

  bool reverseDependencyRemoved = removeDependencyListEntry(reverseDependencies, &dependencySource);
  assert(reverseDependencyRemoved);
  if (reverseDependencies->len == 0) {
    // Clean up list if it is now empty.
    g_hash_table_remove(world->reverseDependencies, &dependencyTarget);
  }

  world->numComponentDependencies--;

  // Only print dependencies to other entities. Otherwise, we print all dummy dependencies removed
  // from newly created components which makes for spammy and misleading logs.
  if (component->entityId != targetEntityId) {
    shovelerLogTrace(
        "Removed dependency from component '%s' of entity %lld to component '%s' of entity %lld.",
        component->type->id,
        component->entityId,
        targetComponentTypeId,
        targetEntityId);
  }

  if (world->callbacks->onRemoveDependency != NULL) {
    world->callbacks->onRemoveDependency(
        world, &dependencySource, &dependencyTarget, world->callbacks->userData);
  }

  return true;
}

static void forEachReverseDependency(
    ShovelerComponent* targetComponent,
    ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction* callbackFunction,
    void* callbackUserData,
    void* worldPointer) {
  ShovelerWorld* world = (ShovelerWorld*) worldPointer;

  ShovelerEntityComponentId dependencyTarget =
      shovelerEntityComponentId(targetComponent->entityId, targetComponent->type->id);

  GArray* reverseDependencies = g_hash_table_lookup(world->reverseDependencies, &dependencyTarget);
  if (reverseDependencies != NULL) {
    for (int i = 0; i < reverseDependencies->len; i++) {
      const ShovelerEntityComponentId* dependencySource =
          &g_array_index(reverseDependencies, ShovelerEntityComponentId, i);

      ShovelerWorldEntity* sourceEntity =
          g_hash_table_lookup(world->entities, &dependencySource->entityId);
      if (sourceEntity != NULL) {
        ShovelerComponent* sourceComponent =
            g_hash_table_lookup(sourceEntity->components, dependencySource->componentTypeId);
        if (sourceComponent != NULL) {
          callbackFunction(sourceComponent, targetComponent, callbackUserData);
        }
      }
    }
  }
}

static bool removeDependencyListEntry(
    GArray* dependencyList, const ShovelerEntityComponentId* entry) {
  for (int i = 0; i < dependencyList->len; i++) {
    const ShovelerEntityComponentId* element =
        &g_array_index(dependencyList, ShovelerEntityComponentId, i);
    if (shovelerEntityComponentIdEqual(element, entry)) {
      g_array_remove_index_fast(dependencyList, i);
      return true;
    }
  }
  return false;
}

static void freeEntity(void* entityPointer) {
  ShovelerWorldEntity* entity = entityPointer;

  g_hash_table_destroy(entity->authoritativeComponents);
  g_hash_table_destroy(entity->components);
  free(entity->label);
  free(entity);
}

static void freeComponent(void* componentPointer) {
  ShovelerComponent* component = componentPointer;
  shovelerComponentFree(component);
}

static void freeDependencyArray(void* dependencyArrayPointer) {
  GArray* dependencyArray = dependencyArrayPointer;

  g_array_free(dependencyArray, /* freeSegment */ true);
}
