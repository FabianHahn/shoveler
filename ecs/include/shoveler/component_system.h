#ifndef SHOVELER_COMPONENT_SYSTEM_H
#define SHOVELER_COMPONENT_SYSTEM_H

#include <stdbool.h>

typedef struct ShovelerComponentFieldStruct ShovelerComponentField;
typedef struct ShovelerComponentFieldValueStruct ShovelerComponentFieldValue;
typedef struct ShovelerComponentStruct ShovelerComponent;
typedef struct ShovelerComponentSystemAdapterStruct ShovelerComponentSystemAdapter;
typedef struct ShovelerComponentSystemStruct ShovelerComponentSystem;
typedef struct ShovelerComponentTypeStruct ShovelerComponentType;
typedef struct ShovelerSystemStruct ShovelerSystem;

typedef bool(ShovelerComponentSystemLiveUpdateFieldFunction)(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData);
typedef bool(ShovelerComponentSystemLiveUpdateDependencyFieldFunction)(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* userData);
typedef void*(ShovelerComponentSystemActivateComponentFunction)(
    ShovelerComponent* component, void* userData);
typedef bool(ShovelerComponentSystemUpdateComponentFunction)(
    ShovelerComponent* component, double dt, void* userData);
typedef void(ShovelerComponentSystemDeactivateComponentFunction)(
    ShovelerComponent* component, void* userData);

typedef struct ShovelerComponentSystemFieldOptionsStruct {
  ShovelerComponentSystemLiveUpdateFieldFunction* liveUpdateField;
  ShovelerComponentSystemLiveUpdateDependencyFieldFunction* liveUpdateDependencyField;
} ShovelerComponentSystemFieldOptions;

typedef struct ShovelerComponentSystemStruct {
  ShovelerComponentSystemAdapter* componentAdapter;
  ShovelerSystem *system;
  ShovelerComponentType* componentType;
  bool requiresAuthority;
  ShovelerComponentSystemFieldOptions* fieldOptions;
  ShovelerComponentSystemActivateComponentFunction* activateComponent;
  ShovelerComponentSystemUpdateComponentFunction* updateComponent;
  ShovelerComponentSystemDeactivateComponentFunction* deactivateComponent;
  void* callbackUserData;
} ShovelerComponentSystem;

ShovelerComponentSystem* shovelerComponentSystemCreate(
    ShovelerSystem *system,
    ShovelerComponentType* componentType);
void shovelerComponentSystemFree(ShovelerComponentSystem* componentSystem);

/**
 * A ShovelerComponentSystemLiveUpdateFieldFunction that does nothing and doesn't propagate the
 * update to reverse dependencies.
 */
static inline bool shovelerComponentSystemLiveUpdateFieldNoop(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData) {
  // deliberately do nothing
  return false;
}

/**
 * A ShovelerComponentSystemLiveUpdateFieldFunction that does nothiung and propagates the update
 * to reverse dependencies.
 */
static inline bool shovelerComponentSystemLiveUpdateFieldPropagate(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData) {
  // deliberately do nothing
  return true;
}

/**
 * A ShovelerComponentSystemLiveUpdateDependencyFieldFunction that does nothing and doesn't
 * propagate the update to reverse dependencies.
 */
static inline bool shovelerComponentSystemLiveUpdateDependencyFieldNoop(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* userData) {
  // deliberately do nothing
  return false;
}

/**
 * A ShovelerComponentSystemLiveUpdateDependencyFieldFunction that does nothiung and propagates the
 * update to reverse dependencies.
 */
static inline bool shovelerComponentSystemLiveUpdateDependencyFieldPropagate(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* userData) {
  // deliberately do nothing
  return true;
}

#endif
