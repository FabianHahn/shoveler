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
    ShovelerComponentWorldAdapter; // forward declaration: below

typedef void(ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction)(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* userData);

// Adapter struct to make a component integrate with a world.
typedef struct ShovelerComponentWorldAdapterStruct {
  ShovelerComponent* (*getComponent)(
      ShovelerComponent* component,
      long long int entityId,
      const char* componentTypeId,
      void* userData);
  void (*forEachReverseDependency)(
      ShovelerComponent* component,
      ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction* callbackFunction,
      void* callbackUserData,
      void* adapterUserData);
  void (*addDependency)(
      ShovelerComponent* component,
      long long int targetEntityId,
      const char* targetComponentTypeId,
      void* userData);
  bool (*removeDependency)(
      ShovelerComponent* component,
      long long int targetEntityId,
      const char* targetComponentTypeId,
      void* userData);

  void (*onUpdateComponentField)(
      ShovelerComponent* component,
      int fieldId,
      const ShovelerComponentField* field,
      const ShovelerComponentFieldValue* value,
      bool isAuthoritative,
      void* userData);
  void (*onActivateComponent)(ShovelerComponent* component, void* userData);
  void (*onDeactivateComponent)(ShovelerComponent* component, void* userData);

  void* userData;
} ShovelerComponentWorldAdapter;

// Adapter struct to make a component integrate with a system.
typedef struct ShovelerComponentSystemAdapterStruct {
  /** Returns true if the specified component requires authority to be activated in this system. */
  bool (*requiresAuthority)(ShovelerComponent* component, void* userData);
  /**
   * If the specified field's value can be live updated, this indicates that the component can be
   * updated by calling liveUpdateField instead of deactivating and reactivating it.
   */
  bool (*canLiveUpdateField)(
      ShovelerComponent* component,
      int fieldId,
      const ShovelerComponentField* field,
      void* userData);
  /**
   * If the dependency of a field can be live updated, this indicates that the component can be
   * updated by calling liveUpdateDependencyField instead of deactivating and reactivating it.
   */
  bool (*canLiveUpdateDependencyField)(
      ShovelerComponent* component,
      int fieldId,
      const ShovelerComponentField* field,
      void* userData);
  /** Returns true if the update should be propagated down to its reverse dependencies. */
  bool (*liveUpdateField)(
      ShovelerComponent* component,
      int fieldId,
      const ShovelerComponentField* field,
      ShovelerComponentFieldValue* fieldValue,
      void* userData);
  /** Returns true if the update should be propagated down to its reverse dependencies. */
  bool (*liveUpdateDependencyField)(
      ShovelerComponent* component,
      int fieldId,
      const ShovelerComponentField* field,
      ShovelerComponent* dependencyComponent,
      void* userData);
  /** Activates the specified component, returning non-NULL data if successful. */
  void* (*activateComponent)(ShovelerComponent* component, void* userData);
  /**
   * Updates a component for the specified component if it is active.
   *
   * In case the component is updated and the function returns true, this update is then propagated
   * recursively to other components depending on it.
   */
  bool (*updateComponent)(ShovelerComponent* component, double dt, void* userData);
  /** Deactivates the specified component and frees its data. */
  void (*deactivateComponent)(ShovelerComponent* component, void* userData);
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

typedef enum {
  SHOVELER_COMPONENT_ACTIVATE_SUCCESS,
  SHOVELER_COMPONENT_ACTIVATE_ALREADY_ACTIVE,
  SHOVELER_COMPONENT_ACTIVATE_NOT_AUTHORITATIVE,
  SHOVELER_COMPONENT_ACTIVATE_DEPENDENCIES_INACTIVE,
  SHOVELER_COMPONENT_ACTIVATE_ACTIVATION_FAILURE
} ShovelerComponentActivateStatus;

typedef enum {
  SHOVELER_COMPONENT_UPDATE_FIELD_SUCCESS,
  SHOVELER_COMPONENT_UPDATE_FIELD_TYPE_MISMATCH,
  SHOVELER_COMPONENT_UPDATE_FIELD_NOT_AUTHORITATIVE
} ShovelerComponentUpdateFieldStatus;

ShovelerComponent* shovelerComponentCreate(
    ShovelerComponentWorldAdapter* worldAdapter,
    ShovelerComponentSystemAdapter* systemAdapter,
    long long int entityId,
    ShovelerComponentType* componentType);
ShovelerComponentActivateStatus shovelerComponentActivate(ShovelerComponent* component);
void shovelerComponentDeactivate(ShovelerComponent* component);
/**
 * Updates a configuration option with the specified id on this component.
 *
 * If isAuthoritative is true, no authority check is performed and the update is always applied.
 */
ShovelerComponentUpdateFieldStatus shovelerComponentUpdateField(
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

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldEntityId(
    ShovelerComponent* component, int id, long long int entityIdValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID;
  value.isSet = true;
  value.entityIdValue = entityIdValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldEntityIdArray(
    ShovelerComponent* component, int id, const long long int* entityIds, size_t size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY;
  value.isSet = true;
  value.entityIdArrayValue.entityIds = (long long int*) entityIds; // won't be modified
  value.entityIdArrayValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldFloat(
    ShovelerComponent* component, int id, float floatValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_FLOAT;
  value.isSet = true;
  value.floatValue = floatValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldBool(
    ShovelerComponent* component, int id, bool boolValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BOOL;
  value.isSet = true;
  value.boolValue = boolValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldInt(
    ShovelerComponent* component, int id, int intValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_INT;
  value.isSet = true;
  value.intValue = intValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldString(
    ShovelerComponent* component, int id, const char* stringValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_STRING;
  value.isSet = true;
  value.stringValue = (char*) stringValue; // won't be modified
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldVector2(
    ShovelerComponent* component, int id, ShovelerVector2 vector2Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2;
  value.isSet = true;
  value.vector2Value = vector2Value;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldVector3(
    ShovelerComponent* component, int id, ShovelerVector3 vector3Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3;
  value.isSet = true;
  value.vector3Value = vector3Value;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldVector4(
    ShovelerComponent* component, int id, ShovelerVector4 vector4Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4;
  value.isSet = true;
  value.vector4Value = vector4Value;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateFieldBytes(
    ShovelerComponent* component, int id, const unsigned char* data, size_t size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BYTES;
  value.isSet = true;
  value.bytesValue.data = (unsigned char*) data; // won't be modified
  value.bytesValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ false);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldEntityId(
    ShovelerComponent* component, int id, long long int entityIdValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID;
  value.isSet = true;
  value.entityIdValue = entityIdValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldEntityIdArray(
    ShovelerComponent* component, int id, const long long int* entityIds, int size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY;
  value.isSet = true;
  value.entityIdArrayValue.entityIds = (long long int*) entityIds; // won't be modified
  value.entityIdArrayValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldFloat(
    ShovelerComponent* component, int id, float floatValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_FLOAT;
  value.isSet = true;
  value.floatValue = floatValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldBool(
    ShovelerComponent* component, int id, bool boolValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BOOL;
  value.isSet = true;
  value.boolValue = boolValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldInt(
    ShovelerComponent* component, int id, int intValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_INT;
  value.isSet = true;
  value.intValue = intValue;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldString(
    ShovelerComponent* component, int id, const char* stringValue) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_STRING;
  value.isSet = true;
  value.stringValue = (char*) stringValue; // won't be modified
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldVector2(
    ShovelerComponent* component, int id, ShovelerVector2 vector2Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2;
  value.isSet = true;
  value.vector2Value = vector2Value;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldVector3(
    ShovelerComponent* component, int id, ShovelerVector3 vector3Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3;
  value.isSet = true;
  value.vector3Value = vector3Value;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldVector4(
    ShovelerComponent* component, int id, ShovelerVector4 vector4Value) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4;
  value.isSet = true;
  value.vector4Value = vector4Value;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
}

static inline ShovelerComponentUpdateFieldStatus shovelerComponentUpdateCanonicalFieldBytes(
    ShovelerComponent* component, int id, const unsigned char* data, int size) {
  ShovelerComponentFieldValue value;
  value.type = SHOVELER_COMPONENT_FIELD_TYPE_BYTES;
  value.isSet = true;
  value.bytesValue.data = (unsigned char*) data; // won't be modified
  value.bytesValue.size = size;
  return shovelerComponentUpdateField(component, id, &value, /* isAuthoritative */ true);
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
