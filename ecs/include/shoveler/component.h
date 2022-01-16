#ifndef SHOVELER_COMPONENT_H
#define SHOVELER_COMPONENT_H

#include <assert.h> // assert
#include <glib.h>
#include <shoveler/component_field.h>
#include <shoveler/types.h>
#include <stdbool.h> // bool

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: below
typedef struct ShovelerComponentTypeStruct
    ShovelerComponentType; // forward declaration: component_type.h
typedef struct ShovelerComponentWorldAdapterStruct
    ShovelerComponentWorldAdapter; // forward delcaration: below

typedef void(ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction)(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* userData);

typedef ShovelerComponent*(ShovelerComponentWorldAdapterGetComponentFunction)(
    ShovelerComponent* component,
    long long int entityId,
    const char* componentTypeId,
    void* userData);
typedef void(ShovelerComponentWorldAdapterUpdateAuthoritativeComponentFunction)(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* userData);
typedef void(ShovelerComponentWorldAdapterAddDependencyFunction)(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* userData);
typedef bool(ShovelerComponentWorldAdapterRemoveDependencyFunction)(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* userData);
typedef void(ShovelerComponentWorldAdapterForEachReverseDependencyFunction)(
    ShovelerComponent* component,
    ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction* callbackFunction,
    void* callbackUserData,
    void* adapterUserData);

// Adapter struct to make a component integrate with a world.
typedef struct ShovelerComponentWorldAdapterStruct {
  ShovelerComponentWorldAdapterGetComponentFunction* getComponent;
  ShovelerComponentWorldAdapterUpdateAuthoritativeComponentFunction* updateAuthoritativeComponent;
  ShovelerComponentWorldAdapterAddDependencyFunction* addDependency;
  ShovelerComponentWorldAdapterRemoveDependencyFunction* removeDependency;
  ShovelerComponentWorldAdapterForEachReverseDependencyFunction* forEachReverseDependency;
  void* userData;
} ShovelerComponentWorldAdapter;

typedef bool(ShovelerComponentSystemAdapterRequiresAuthorityFunction)(
    ShovelerComponent* component, void* userData);
typedef bool(ShovelerComponentSystemAdapterCanLiveUpdateFieldFunction)(
    ShovelerComponent* component, int fieldId, const ShovelerComponentField* field, void* userData);
typedef bool(ShovelerComponentSystemAdapterCanLiveUpdateDependencyFieldFunction)(
    ShovelerComponent* component, int fieldId, const ShovelerComponentField* field, void* userData);
typedef bool(ShovelerComponentSystemAdapterLiveUpdateFieldFunction)(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData);
typedef bool(ShovelerComponentSystemAdapterLiveUpdateDependencyFieldFunction)(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* userData);
typedef void*(ShovelerComponentSystemAdapterActivateComponentFunction)(
    ShovelerComponent* component, void* userData);
typedef bool(ShovelerComponentSystemAdapterUpdateComponentFunction)(
    ShovelerComponent* component, double dt, void* userData);
typedef void(ShovelerComponentSystemAdapterDeactivateComponentFunction)(
    ShovelerComponent* component, void* userData);

// Adapter struct to make a component integrate with a system.
typedef struct ShovelerComponentSystemAdapterStruct {
  /** Returns true if the specified component requries authority to be activated in this system. */
  ShovelerComponentSystemAdapterRequiresAuthorityFunction* requiresAuthority;
  /**
   * If the specified field can be live updated, this indicates that the component can be
   * updated by calling liveUpdateField instead of deactivating and reactivating it.
   */
  ShovelerComponentSystemAdapterCanLiveUpdateFieldFunction* canLiveUpdateField;
  /**
   * If the specified dependency field can be live updated, this indicates that the component
   * can be updated by calling liveUpdateDependencyField instead of deactivating and
   * reactivating it.
   */
  ShovelerComponentSystemAdapterCanLiveUpdateFieldFunction* canLiveUpdateDependencyField;
  /** Returns true if the update should be propagated down to its reverse dependencies. */
  ShovelerComponentSystemAdapterLiveUpdateFieldFunction* liveUpdateField;
  /** Returns true if the update should be propagated down to its reverse dependencies. */
  ShovelerComponentSystemAdapterLiveUpdateDependencyFieldFunction* liveUpdateDependencyField;
  /** Activates the specified component, returning non-NULL data if successful. */
  ShovelerComponentSystemAdapterActivateComponentFunction* activateComponent;
  /**
   * Updates a component for the specified component if it is active.
   *
   * In case the component is updated and the functin returns true, this update is then propagated
   * recursively to other components depending on it.
   */
  ShovelerComponentSystemAdapterUpdateComponentFunction* updateComponent;
  /** Deactivates the specified component and frees its data. */
  ShovelerComponentSystemAdapterDeactivateComponentFunction* deactivateComponent;
  void* userData;
} ShovelerComponentSystemAdapter;

typedef struct ShovelerComponentStruct {
  ShovelerComponentWorldAdapter* worldAdapter;
  ShovelerComponentSystemAdapter* systemAdapter;
  long long int entityId;
  ShovelerComponentType* type;
  bool isAuthoritative;
  ShovelerComponentFieldValue* fieldValues;
  // array of ShovelerEntityComponentId
  GArray* dependencies;
  void* systemData;
} ShovelerComponent;

ShovelerComponent* shovelerComponentCreate(
    ShovelerComponentWorldAdapter* worldAdapter,
    ShovelerComponentSystemAdapter* systemAdapter,
    long long int entityId,
    ShovelerComponentType* componentType);
bool shovelerComponentActivate(ShovelerComponent* component);
void shovelerComponentDeactivate(ShovelerComponent* component);
/**
 * Updates a configuration option with the specified id on this component.
 *
 * If isCanonical is true, no authority check is performed and the update is applied without
 * invoking the authoritative update handler.
 */
bool shovelerComponentUpdateField(
    ShovelerComponent* component,
    int id,
    const ShovelerComponentFieldValue* value,
    bool isCanonical);
bool shovelerComponentClearField(ShovelerComponent* component, int id, bool isCanonical);
const ShovelerComponentFieldValue* shovelerComponentGetFieldValue(
    ShovelerComponent* component, int id);
bool shovelerComponentIsActive(ShovelerComponent* component);
bool shovelerComponentUpdate(ShovelerComponent* component, double dt);
void shovelerComponentDelegate(ShovelerComponent* component);
bool shovelerComponentIsAuthoritative(ShovelerComponent* component);
void shovelerComponentUndelegate(ShovelerComponent* component);
ShovelerComponent* shovelerComponentGetDependency(ShovelerComponent* component, int fieldId);
ShovelerComponent* shovelerComponentGetArrayDependency(
    ShovelerComponent* component, int fieldId, int index);
void shovelerComponentFree(ShovelerComponent* component);

/**
 * A ShovelerComponentFieldLiveUpdateFunction that does nothing and can be
 * passed to ShovelerComponentField. It doesn't propagate the update.
 */
static inline bool shovelerComponentLiveUpdateNoop(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value) {
  // deliberately do nothing
  return false;
}

/**
 * A ShovelerComponentFieldLiveUpdateFunction that does propagates the update
 * to reverse dependencies but otherwise does nothing. It can be passed to
 * ShovelerComponentField.
 */
static inline bool shovelerComponentLiveUpdatePropagate(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value) {
  // deliberately do nothing
  return true;
}

/**
 * A ShovelerComponentFieldLiveUpdateDependencyFunction that does nothing and can
 * be passed to ShovelerComponentFieldDependency. It doesn't propagate the update.
 */
static inline bool shovelerComponentLiveUpdateDependencyNoop(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent) {
  // deliberately do nothing
  return false;
}

/**
 * A ShovelerComponentFieldLiveUpdateDependencyFunction that does propagates the update
 * to reverse dependencies but otherwise does nothing. It can be passed to
 * ShovelerComponentFieldDependency.
 */
static inline bool shovelerComponentLiveUpdateDependencyPropagate(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent) {
  // deliberately do nothing
  return true;
}

static inline bool shovelerComponentUpdateFieldEntityId(
    ShovelerComponent* component, int id, long long int entityIdValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID;
  value.isSet = true;
  value.entityIdValue = entityIdValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldEntityIdArray(
    ShovelerComponent* component, int id, const long long int* entityIds, size_t size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY;
  value.isSet = true;
  value.entityIdArrayValue.entityIds = (long long int*) entityIds; // won't be modified
  value.entityIdArrayValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldFloat(
    ShovelerComponent* component, int id, float floatValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_FLOAT;
  value.isSet = true;
  value.floatValue = floatValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldBool(
    ShovelerComponent* component, int id, bool boolValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BOOL;
  value.isSet = true;
  value.boolValue = boolValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldInt(
    ShovelerComponent* component, int id, int intValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_INT;
  value.isSet = true;
  value.intValue = intValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldString(
    ShovelerComponent* component, int id, const char* stringValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_STRING;
  value.isSet = true;
  value.stringValue = (char*) stringValue; // won't be modified
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldVector2(
    ShovelerComponent* component, int id, ShovelerVector2 vector2Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2;
  value.isSet = true;
  value.vector2Value = vector2Value;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldVector3(
    ShovelerComponent* component, int id, ShovelerVector3 vector3Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3;
  value.isSet = true;
  value.vector3Value = vector3Value;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldVector4(
    ShovelerComponent* component, int id, ShovelerVector4 vector4Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4;
  value.isSet = true;
  value.vector4Value = vector4Value;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateFieldBytes(
    ShovelerComponent* component, int id, const unsigned char* data, size_t size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BYTES;
  value.isSet = true;
  value.bytesValue.data = (unsigned char*) data; // won't be modified
  value.bytesValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateCanonicalFieldEntityId(
    ShovelerComponent* component, int id, long long int entityIdValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID;
  value.isSet = true;
  value.entityIdValue = entityIdValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldEntityIdArray(
    ShovelerComponent* component, int id, const long long int* entityIds, int size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY;
  value.isSet = true;
  value.entityIdArrayValue.entityIds = (long long int*) entityIds; // won't be modified
  value.entityIdArrayValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldFloat(
    ShovelerComponent* component, int id, float floatValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_FLOAT;
  value.isSet = true;
  value.floatValue = floatValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldBool(
    ShovelerComponent* component, int id, bool boolValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BOOL;
  value.isSet = true;
  value.boolValue = boolValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldInt(
    ShovelerComponent* component, int id, int intValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_INT;
  value.isSet = true;
  value.intValue = intValue;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldString(
    ShovelerComponent* component, int id, const char* stringValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_STRING;
  value.isSet = true;
  value.stringValue = (char*) stringValue; // won't be modified
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldVector2(
    ShovelerComponent* component, int id, ShovelerVector2 vector2Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2;
  value.isSet = true;
  value.vector2Value = vector2Value;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldVector3(
    ShovelerComponent* component, int id, ShovelerVector3 vector3Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3;
  value.isSet = true;
  value.vector3Value = vector3Value;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldVector4(
    ShovelerComponent* component, int id, ShovelerVector4 vector4Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4;
  value.isSet = true;
  value.vector4Value = vector4Value;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalFieldBytes(
    ShovelerComponent* component, int id, const unsigned char* data, int size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BYTES;
  value.isSet = true;
  value.bytesValue.data = (unsigned char*) data; // won't be modified
  value.bytesValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentHasFieldValue(ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);

  return fieldValue->isSet;
}

static inline long long int shovelerComponentGetFieldValueEntityId(
    ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID);

  return fieldValue->entityIdValue;
}

static inline void shovelerComponentGetFieldValueEntityIdArray(
    ShovelerComponent* component,
    int id,
    const long long int** outputEntityIds,
    size_t* outputSize) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY);

  *outputEntityIds = fieldValue->entityIdArrayValue.entityIds;
  *outputSize = fieldValue->entityIdArrayValue.size;
}

static inline float shovelerComponentGetFieldValueFloat(ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_FLOAT);

  return fieldValue->floatValue;
}

static inline bool shovelerComponentGetFieldValueBool(ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_BOOL);

  return fieldValue->boolValue;
}

static inline int shovelerComponentGetFieldValueInt(ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_INT);

  return fieldValue->intValue;
}

static inline const char* shovelerComponentGetFieldValueString(
    ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_STRING);

  return fieldValue->stringValue;
}

static inline ShovelerVector2 shovelerComponentGetFieldValueVector2(
    ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2);

  return fieldValue->vector2Value;
}

static inline ShovelerVector3 shovelerComponentGetFieldValueVector3(
    ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3);

  return fieldValue->vector3Value;
}

static inline ShovelerVector4 shovelerComponentGetFieldValueVector4(
    ShovelerComponent* component, int id) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4);

  return fieldValue->vector4Value;
}

static inline void shovelerComponentGetFieldValueBytes(
    ShovelerComponent* component, int id, const unsigned char** outputData, int* outputSize) {
  const ShovelerComponentFieldValue* fieldValue = shovelerComponentGetFieldValue(component, id);
  assert(fieldValue->isSet);
  assert(fieldValue->type == SHOVELER_COMPONENT_FIELD_TYPE_BYTES);

  if (outputData != NULL) {
    *outputData = fieldValue->bytesValue.data;
  }

  if (outputSize != NULL) {
    *outputSize = fieldValue->bytesValue.size;
  }
}

#endif
