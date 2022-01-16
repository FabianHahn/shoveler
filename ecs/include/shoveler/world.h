#ifndef SHOVELER_WORLD_H
#define SHOVELER_WORLD_H

#include <glib.h>

typedef struct ShovelerComponentStruct ShovelerComponent;
typedef struct ShovelerComponentFieldStruct ShovelerComponentField;
typedef struct ShovelerComponentFieldValueStruct ShovelerComponentFieldValue;
typedef struct ShovelerComponentSystemAdapterStruct ShovelerComponentSystemAdapter;
typedef struct ShovelerComponentTypeStruct ShovelerComponentType;
typedef struct ShovelerComponentWorldAdapterStruct ShovelerComponentWorldAdapter;
typedef struct ShovelerEntityComponentIdStruct ShovelerEntityComponentId;
typedef struct ShovelerSchemaStruct ShovelerSchema;
typedef struct ShovelerSystemStruct ShovelerSystem;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef void(ShovelerWorldUpdateAuthoritativeComponentFunction)(
    ShovelerWorld* world,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* userData);

typedef struct ShovelerWorldComponentTypeEntryStruct {
  ShovelerComponentType* componentType;
  ShovelerComponentSystemAdapter* componentSystemAdapter;
} ShovelerWorldComponentTypeEntry;

typedef struct ShovelerWorldStruct {
  /** map from entity id (long long int) to entities (ShovelerWorldEntity *) */
  GHashTable* entities;
  /** map from source (ShovelerEntityComponentId *) to array of (ShovelerEntityComponentId *) */
  GHashTable* dependencies;
  /** map from target (ShovelerEntityComponentId *) to array of (ShovelerEntityComponentId *) */
  GHashTable* reverseDependencies;
  /** array of (ShovelerWorldDependencyCallback) */
  GArray* dependencyCallbacks;
  ShovelerSchema* schema;
  ShovelerSystem* system;
  ShovelerWorldUpdateAuthoritativeComponentFunction* updateAuthoritativeComponent;
  void* updateAuthoritativeComponentUserData;
  ShovelerComponentWorldAdapter* componentWorldAdapter;
  int numComponentDependencies;
  int numComponents;
} ShovelerWorld;

typedef struct ShovelerWorldEntityStruct {
  ShovelerWorld* world;
  long long int id;
  char* label;
  /** map from string component type name to (ShovelerComponent *) */
  /* private */ GHashTable* components;
  /** set of component type names */
  /* private */ GHashTable* authoritativeComponents;
} ShovelerWorldEntity;

typedef void(ShovelerWorldDependencyCallbackFunction)(
    ShovelerWorld* world,
    const ShovelerEntityComponentId* dependencySource,
    const ShovelerEntityComponentId* dependencyTarget,
    bool added,
    void* userData);

typedef struct {
  ShovelerWorldDependencyCallbackFunction* function;
  void* userData;
} ShovelerWorldDependencyCallback;

ShovelerWorld* shovelerWorldCreate(
    ShovelerSchema* schema,
    ShovelerSystem* system,
    ShovelerWorldUpdateAuthoritativeComponentFunction* updateAuthoritativeComponent,
    void* updateAuthoritativeComponentUserData);
ShovelerComponentWorldAdapter* shovelerWorldGetComponentAdapter(ShovelerWorld* world);
ShovelerWorldEntity* shovelerWorldAddEntity(ShovelerWorld* world, long long int entityId);
bool shovelerWorldRemoveEntity(ShovelerWorld* world, long long int entityId);
ShovelerComponent* shovelerWorldEntityAddComponent(
    ShovelerWorldEntity* entity, const char* componentTypeId);
bool shovelerWorldEntityRemoveComponent(ShovelerWorldEntity* entity, const char* componentTypeId);
void shovelerWorldEntityDelegateComponent(ShovelerWorldEntity* entity, const char* componentTypeId);
bool shovelerWorldEntityIsAuthoritative(ShovelerWorldEntity* entity, const char* componentTypeId);
void shovelerWorldEntityUndelegateComponent(
    ShovelerWorldEntity* entity, const char* componentTypeId);
const ShovelerWorldDependencyCallback* shovelerWorldAddDependencyCallback(
    ShovelerWorld* world, ShovelerWorldDependencyCallbackFunction* function, void* userData);
bool shovelerWorldRemoveDependencyCallback(
    ShovelerWorld* world, const ShovelerWorldDependencyCallback* callback);
void shovelerWorldFree(ShovelerWorld* world);

static inline ShovelerWorldEntity* shovelerWorldGetEntity(
    ShovelerWorld* world, long long int entityId) {
  return (ShovelerWorldEntity*) g_hash_table_lookup(world->entities, &entityId);
}

static inline ShovelerComponent* shovelerWorldEntityGetComponent(
    ShovelerWorldEntity* entity, const char* componentTypeId) {
  return (ShovelerComponent*) g_hash_table_lookup(entity->components, componentTypeId);
}

#endif
