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
typedef struct ShovelerWorldEntityStruct ShovelerWorldEntity;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef struct ShovelerWorldCallbacksStruct {
  void (*onAddEntity)(ShovelerWorld* world, ShovelerWorldEntity* entity, void* userData);
  void (*onRemoveEntity)(ShovelerWorld* world, long long int entityId, void* userData);
  void (*onAddComponent)(
      ShovelerWorld* world,
      ShovelerWorldEntity* entity,
      ShovelerComponent* component,
      void* userData);
  /**
   * Called after the field value has been updated, but before any potential side effect have been
   * applied:
   *  * If the field cannot be live updated, the component (and its reverse dependencies) have
   *    already been deactivated but not reactivated yet.
   *  * If the field can be Live field updated, the live update callback has not been called yet.
   *  * For a dependency field, previous dependencies have been removed but new ones have not been
   *    added yet.
   *
   * If isAuthoritative is true, that means we're dealing with a non-canonical update and the
   * component is delegated, i.e. it should be forwarded to an authoritative server.
   */
  void (*onUpdateComponent)(
      ShovelerWorld* world,
      ShovelerWorldEntity* entity,
      ShovelerComponent* component,
      int fieldId,
      const ShovelerComponentField* field,
      const ShovelerComponentFieldValue* value,
      bool isAuthoritative,
      void* userData);
  /**
   * Called after the component has been activated, but before any reverse dependencies are
   * recursively activated.
   */
  void (*onActivateComponent)(
      ShovelerWorld* world,
      ShovelerWorldEntity* entity,
      ShovelerComponent* component,
      void* userData);
  /**
   * Called after the component has been deactivated, which means that all reverse dependencies have
   * also already been recursively deactivated.
   */
  void (*onDeactivateComponent)(
      ShovelerWorld* world,
      ShovelerWorldEntity* entity,
      ShovelerComponent* component,
      void* userData);
  void (*onDelegateComponent)(
      ShovelerWorld* world,
      ShovelerWorldEntity* entity,
      const char* componentTypeId,
      void* userData);
  void (*onUndelegateComponent)(
      ShovelerWorld* world,
      ShovelerWorldEntity* entity,
      const char* componentTypeId,
      void* userData);
  void (*onRemoveComponent)(
      ShovelerWorld* world,
      ShovelerWorldEntity* entity,
      const char* componentTypeId,
      void* userData);
  void (*onAddDependency)(
      ShovelerWorld* world,
      const ShovelerEntityComponentId* dependencySource,
      const ShovelerEntityComponentId* dependencyTarget,
      void* userData);
  void (*onRemoveDependency)(
      ShovelerWorld* world,
      const ShovelerEntityComponentId* dependencySource,
      const ShovelerEntityComponentId* dependencyTarget,
      void* userData);
  void* userData;
} ShovelerWorldCallbacks;

ShovelerWorldCallbacks shovelerWorldCallbacks();

typedef struct ShovelerWorldStruct {
  /** map from entity id (long long int) to entities (ShovelerWorldEntity *) */
  GHashTable* entities;
  /** map from source (ShovelerEntityComponentId *) to array of (ShovelerEntityComponentId *) */
  GHashTable* dependencies;
  /** map from target (ShovelerEntityComponentId *) to array of (ShovelerEntityComponentId *) */
  GHashTable* reverseDependencies;
  ShovelerSchema* schema;
  ShovelerSystem* system;
  ShovelerWorldCallbacks* callbacks;
  ShovelerComponentWorldAdapter* componentWorldAdapter;
  int numComponentDependencies;
  int numComponents;
} ShovelerWorld;

typedef struct ShovelerWorldEntityStruct {
  ShovelerWorld* world;
  long long int id;
  char* label;
  /** map from string component type ID to (ShovelerComponent *) */
  /* private */ GHashTable* components;
  /** set of component type IDs */
  /* private */ GHashTable* authoritativeComponents;
} ShovelerWorldEntity;

typedef enum {
  SHOVELER_WORLD_ENTITY_ADD_COMPONENT_SUCCESS,
  SHOVELER_WORLD_ENTITY_ADD_COMPONENT_INVALID_TYPE,
  SHOVELER_WORLD_ENTITY_ADD_COMPONENT_ALREADY_EXISTS,
} ShovelerWorldEntityAddComponentStatus;

/** Caller retains ownership over passed objects. */
ShovelerWorld* shovelerWorldCreate(
    ShovelerSchema* schema, ShovelerSystem* system, ShovelerWorldCallbacks* callbacks);
ShovelerComponentWorldAdapter* shovelerWorldGetComponentAdapter(ShovelerWorld* world);
ShovelerWorldEntity* shovelerWorldAddEntity(ShovelerWorld* world, long long int entityId);
bool shovelerWorldRemoveEntity(ShovelerWorld* world, long long int entityId);
ShovelerComponent* shovelerWorldEntityAddComponent(
    ShovelerWorldEntity* entity,
    const char* componentTypeId,
    ShovelerWorldEntityAddComponentStatus* status);
bool shovelerWorldEntityRemoveComponent(ShovelerWorldEntity* entity, const char* componentTypeId);
void shovelerWorldEntityDelegateComponent(ShovelerWorldEntity* entity, const char* componentTypeId);
bool shovelerWorldEntityIsAuthoritative(ShovelerWorldEntity* entity, const char* componentTypeId);
void shovelerWorldEntityUndelegateComponent(
    ShovelerWorldEntity* entity, const char* componentTypeId);
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
