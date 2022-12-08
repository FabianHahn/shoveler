#include "shoveler/component.h"

#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // strdup memcpy memset

#include "shoveler/component_field.h"
#include "shoveler/component_type.h"
#include "shoveler/entity_component_id.h"
#include "shoveler/log.h"

static void updateReverseDependency(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* unused);
static void activateReverseDependency(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* unused);
static void deactivateReverseDependency(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* unused);
static void addFieldDependencies(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* fieldValue);
static void removeFieldDependencies(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* fieldValue);
static void addDependency(
    ShovelerComponent* component, long long int targetEntityId, const char* targetComponentTypeId);
static void removeDependency(
    ShovelerComponent* component, long long int targetEntityId, const char* targetComponentTypeId);
static bool checkDependenciesActive(ShovelerComponent* component);
static long long int toDependencyTargetEntityId(
    ShovelerComponent* component, long long int entityIdValue);

ShovelerComponent* shovelerComponentCreate(
    ShovelerComponentWorldAdapter* worldAdapter,
    ShovelerComponentSystemAdapter* systemAdapter,
    long long int entityId,
    ShovelerComponentType* componentType) {
  ShovelerComponent* component = malloc(sizeof(ShovelerComponent));
  component->worldAdapter = worldAdapter;
  component->systemAdapter = systemAdapter;
  component->entityId = entityId;
  component->type = componentType;
  component->type = componentType;
  component->isAuthoritative = false;
  component->fieldValues = NULL;
  component->dependencies =
      g_array_new(/* zeroTerminated */ false, /* clear */ true, sizeof(ShovelerEntityComponentId));
  component->systemData = NULL;

  if (component->type->numFields > 0) {
    component->fieldValues =
        malloc((size_t) component->type->numFields * sizeof(ShovelerComponentFieldValue));

    for (int id = 0; id < component->type->numFields; id++) {
      const ShovelerComponentField* field = &component->type->fields[id];
      ShovelerComponentFieldValue* fieldValue = &component->fieldValues[id];

      shovelerComponentFieldInitValue(fieldValue, field->type);

      if (!field->isOptional) {
        fieldValue->isSet = true; // initialize with default value
      }

      addFieldDependencies(component, field, fieldValue);
    }
  }

  return component;
}

bool shovelerComponentActivate(ShovelerComponent* component) {
  if (shovelerComponentIsActive(component)) {
    return true;
  }

  bool requiresAuthority =
      component->systemAdapter->requiresAuthority(component, component->systemAdapter->userData);
  if (requiresAuthority && !component->isAuthoritative) {
    return false;
  }

  if (!checkDependenciesActive(component)) {
    return false;
  }

  component->systemData =
      component->systemAdapter->activateComponent(component, component->systemAdapter->userData);
  if (component->systemData == NULL) {
    return false;
  }

  shovelerLogTrace(
      "Activated component '%s' of entity %lld.", component->type->id, component->entityId);

  component->worldAdapter->onActivateComponent(component, component->worldAdapter->userData);

  component->worldAdapter->forEachReverseDependency(
      component,
      activateReverseDependency,
      /* callbackUserData */ NULL,
      component->worldAdapter->userData);

  return true;
}

void shovelerComponentDeactivate(ShovelerComponent* component) {
  if (component->systemData == NULL) {
    return;
  }

  component->worldAdapter->forEachReverseDependency(
      component,
      deactivateReverseDependency,
      /* callbackUserData */ NULL,
      component->worldAdapter->userData);

  component->systemAdapter->deactivateComponent(component, component->systemAdapter->userData);
  component->systemData = NULL;

  shovelerLogTrace(
      "Deactivated component '%s' of entity %lld.", component->type->id, component->entityId);

  component->worldAdapter->onDeactivateComponent(component, component->worldAdapter->userData);
}

bool shovelerComponentUpdateField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentFieldValue* value,
    bool isCanonical) {
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);

  const ShovelerComponentField* field = &component->type->fields[fieldId];
  ShovelerComponentFieldValue* fieldValue = &component->fieldValues[fieldId];

  if (value != NULL && field->type != value->type) {
    return false;
  }

  if (!isCanonical) {
    if (!component->isAuthoritative) {
      shovelerLogWarning(
          "Skipping non-canonical update to entity %lld component %s field %s(%d) that is not "
          "authoritative.",
          component->entityId,
          component->type->id,
          field->name,
          fieldId);
      return false;
    }
  }

  bool wasActive = component->systemData != NULL;
  bool canLiveUpdate = component->systemAdapter->canLiveUpdateField(
      component, fieldId, field, component->systemAdapter->userData);
  bool isDependencyUpdate = field->dependencyComponentTypeId != NULL;

  if (wasActive && !canLiveUpdate) {
    // cannot live update, so deactivate before updating
    shovelerComponentDeactivate(component);
  }

  if (isDependencyUpdate) {
    // remove the dependencies from the previous field value
    removeFieldDependencies(component, field, fieldValue);
  }

  // update option to its new value
  shovelerComponentFieldAssignValue(fieldValue, value);

  component->worldAdapter->onUpdateComponentField(
      component, fieldId, field, value, !isCanonical, component->worldAdapter->userData);

  if (isDependencyUpdate) {
    // Add the new dependencies, which might deactivate the component if the dependency isn't
    // satisfied.
    addFieldDependencies(component, field, fieldValue);

    // update wasActive because component might have been deactivated
    wasActive = component->systemData != NULL;
  }

  if (wasActive) {
    if (canLiveUpdate) {
      // live update value
      bool propagateUpdate = component->systemAdapter->liveUpdateField(
          component, fieldId, field, fieldValue, component->systemAdapter->userData);

      if (propagateUpdate) {
        // update reverse dependencies
        component->worldAdapter->forEachReverseDependency(
            component,
            updateReverseDependency,
            /* callbackUserData */ NULL,
            component->worldAdapter->userData);
      }
    } else {
      // cannot live update, so try reactivating again
      shovelerComponentActivate(component);
    }
  }

  return true;
}

bool shovelerComponentClearField(ShovelerComponent* component, int fieldId, bool isCanonical) {
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);

  const ShovelerComponentField* field = &component->type->fields[fieldId];

  ShovelerComponentFieldValue fieldValue;
  shovelerComponentFieldInitValue(&fieldValue, field->type);

  return shovelerComponentUpdateField(component, fieldId, &fieldValue, isCanonical);
}

const ShovelerComponentFieldValue* shovelerComponentGetFieldValue(
    ShovelerComponent* component, int fieldId) {
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);

  return &component->fieldValues[fieldId];
}

bool shovelerComponentIsActive(ShovelerComponent* component) {
  return component->systemData != NULL;
}

bool shovelerComponentUpdate(ShovelerComponent* component, double dt) {
  if (!shovelerComponentIsActive(component)) {
    return false;
  }

  if (!component->systemAdapter->updateComponent(
          component, dt, component->systemAdapter->userData)) {
    return false;
  }

  // update reverse dependencies
  component->worldAdapter->forEachReverseDependency(
      component,
      updateReverseDependency,
      /* callbackUserData */ NULL,
      component->worldAdapter->userData);

  return true;
}

void shovelerComponentDelegate(ShovelerComponent* component) { component->isAuthoritative = true; }

bool shovelerComponentIsAuthoritative(ShovelerComponent* component) {
  return component->isAuthoritative;
}

void shovelerComponentUndelegate(ShovelerComponent* component) {
  bool requiresAuthority =
      component->systemAdapter->requiresAuthority(component, component->systemAdapter->userData);
  if (shovelerComponentIsActive(component) && requiresAuthority) {
    shovelerComponentDeactivate(component);
  }

  component->isAuthoritative = false;
}

ShovelerComponent* shovelerComponentGetDependency(ShovelerComponent* component, int fieldId) {
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);

  const ShovelerComponentField* field = &component->type->fields[fieldId];
  ShovelerComponentFieldValue* fieldValue = &component->fieldValues[fieldId];

  if (field->dependencyComponentTypeId == NULL) {
    return NULL;
  }

  if (field->type != SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID) {
    return NULL;
  }

  return component->worldAdapter->getComponent(
      component,
      toDependencyTargetEntityId(component, fieldValue->entityIdValue),
      field->dependencyComponentTypeId,
      component->worldAdapter->userData);
}

ShovelerComponent* shovelerComponentGetArrayDependency(
    ShovelerComponent* component, int fieldId, int index) {
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);

  const ShovelerComponentField* field = &component->type->fields[fieldId];
  ShovelerComponentFieldValue* fieldValue = &component->fieldValues[fieldId];

  if (field->dependencyComponentTypeId == NULL) {
    return NULL;
  }

  if (field->type != SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY) {
    return NULL;
  }

  if (index >= fieldValue->entityIdArrayValue.size) {
    return NULL;
  }

  return component->worldAdapter->getComponent(
      component,
      toDependencyTargetEntityId(component, fieldValue->entityIdArrayValue.entityIds[index]),
      field->dependencyComponentTypeId,
      component->worldAdapter->userData);
}

void shovelerComponentFree(ShovelerComponent* component) {
  if (component == NULL) {
    return;
  }

  shovelerComponentDeactivate(component);

  for (int fieldId = 0; fieldId < component->type->numFields; fieldId++) {
    const ShovelerComponentField* field = &component->type->fields[fieldId];
    ShovelerComponentFieldValue* fieldValue = &component->fieldValues[fieldId];

    removeFieldDependencies(component, field, fieldValue);
  }
  assert(component->dependencies->len == 0);
  g_array_free(component->dependencies, /* freeSegment */ true);

  for (int fieldId = 0; fieldId < component->type->numFields; fieldId++) {
    ShovelerComponentFieldValue* fieldValue = &component->fieldValues[fieldId];
    shovelerComponentFieldClearValue(fieldValue);
  }
  free(component->fieldValues);

  free(component);
}

static void updateReverseDependency(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* unused) {
  if (sourceComponent->systemData == NULL) {
    // no need to update the reverse dependency if it isn't active
    return;
  }

  bool requiresReactivation = false;
  for (int fieldId = 0; fieldId < sourceComponent->type->numFields; fieldId++) {
    const ShovelerComponentField* field = &sourceComponent->type->fields[fieldId];
    ShovelerComponentFieldValue* fieldValue = &sourceComponent->fieldValues[fieldId];

    // check if this option is a dependency pointing to the target
    if (field->dependencyComponentTypeId != targetComponent->type->id) {
      continue;
    }

    if (!fieldValue->isSet) {
      assert(field->isOptional);
      continue;
    }

    if (field->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID) {
      if (toDependencyTargetEntityId(sourceComponent, fieldValue->entityIdValue) !=
          targetComponent->entityId) {
        continue;
      }
    } else {
      assert(field->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY);

      bool requiresUpdate = false;
      for (int i = 0; i < fieldValue->entityIdArrayValue.size; i++) {
        if (toDependencyTargetEntityId(
                sourceComponent, fieldValue->entityIdArrayValue.entityIds[i]) ==
            targetComponent->entityId) {
          requiresUpdate = true;
          break;
        }
      }

      if (!requiresUpdate) {
        continue;
      }
    }

    if (!sourceComponent->systemAdapter->canLiveUpdateDependencyField(
            sourceComponent, fieldId, field, sourceComponent->systemAdapter->userData)) {
      // At least one dependency field on the source component doesn't know how to live
      // update, so we need to deactivate and reactivate the component.
      // This also means that we can stop iterating over the remaining fields, since they
      // will be reinitialized even if they could be live updated and pointed to the same
      // target.
      requiresReactivation = true;
      break;
    }

    bool propagateUpdate = sourceComponent->systemAdapter->liveUpdateDependencyField(
        sourceComponent, fieldId, field, targetComponent, sourceComponent->systemAdapter->userData);
    if (propagateUpdate) {
      // recursively update reverse dependencies
      sourceComponent->worldAdapter->forEachReverseDependency(
          sourceComponent,
          updateReverseDependency,
          /* callbackUserData */ NULL,
          sourceComponent->worldAdapter->userData);
    }
  }

  if (requiresReactivation) {
    // reactivate the source component system
    shovelerComponentDeactivate(sourceComponent);
    shovelerComponentActivate(sourceComponent);
  }
}

static void activateReverseDependency(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* unused) {
  shovelerComponentActivate(sourceComponent);
}

static void deactivateReverseDependency(
    ShovelerComponent* sourceComponent, ShovelerComponent* targetComponent, void* unused) {
  shovelerComponentDeactivate(sourceComponent);
}

static void addFieldDependencies(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* fieldValue) {
  if (!fieldValue->isSet) {
    assert(field->isOptional);
    return;
  }

  if (field->dependencyComponentTypeId == NULL) {
    // this field isn't a dependency
    return;
  }

  assert(field->type == fieldValue->type);

  if (field->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID) {
    addDependency(
        component,
        /* targetEntityId */ toDependencyTargetEntityId(component, fieldValue->entityIdValue),
        /* targetComponentTypeId */ field->dependencyComponentTypeId);
  } else if (field->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY) {
    for (int i = 0; i < fieldValue->entityIdArrayValue.size; i++) {
      addDependency(
          component,
          /* targetEntityId */
          toDependencyTargetEntityId(component, fieldValue->entityIdArrayValue.entityIds[i]),
          /* targetComponentTypeId */ field->dependencyComponentTypeId);
    }
  }

  // If the component is activated and one of the added dependencies isn't, we need to deactivate
  // it.
  if (component->systemData != NULL) {
    if (!checkDependenciesActive(component)) {
      shovelerComponentDeactivate(component);
    }
  }
}

static void removeFieldDependencies(
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* fieldValue) {
  if (!fieldValue->isSet) {
    assert(field->isOptional);
    return;
  }

  if (field->dependencyComponentTypeId == NULL) {
    // this field isn't a dependency
    return;
  }

  assert(field->type == fieldValue->type);

  if (field->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID) {
    removeDependency(
        component,
        /* targetEntityId */ toDependencyTargetEntityId(component, fieldValue->entityIdValue),
        /* targetComponentTypeId */ field->dependencyComponentTypeId);
  } else if (field->type == SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY) {
    for (int i = 0; i < fieldValue->entityIdArrayValue.size; i++) {
      removeDependency(
          component,
          /* targetEntityId */
          toDependencyTargetEntityId(component, fieldValue->entityIdArrayValue.entityIds[i]),
          /* targetComponentTypeId */ field->dependencyComponentTypeId);
    }
  }
}

static void addDependency(
    ShovelerComponent* component, long long int targetEntityId, const char* targetComponentTypeId) {
  ShovelerEntityComponentId dependency;
  dependency.entityId = targetEntityId;
  dependency.componentTypeId = targetComponentTypeId;
  g_array_append_val(component->dependencies, dependency);

  component->worldAdapter->addDependency(
      component, targetEntityId, targetComponentTypeId, component->worldAdapter->userData);
}

static void removeDependency(
    ShovelerComponent* component, long long int targetEntityId, const char* targetComponentTypeId) {
  for (int i = 0; i < component->dependencies->len; i++) {
    const ShovelerEntityComponentId* dependency =
        &g_array_index(component->dependencies, ShovelerEntityComponentId, i);
    if (dependency->entityId == targetEntityId &&
        dependency->componentTypeId == targetComponentTypeId) {
      g_array_remove_index_fast(component->dependencies, i);
      break;
    }
  }

  bool dependencyRemoved = component->worldAdapter->removeDependency(
      component, targetEntityId, targetComponentTypeId, component->worldAdapter->userData);
  assert(dependencyRemoved);
}

static bool checkDependenciesActive(ShovelerComponent* component) {
  for (int i = 0; i < component->dependencies->len; i++) {
    const ShovelerEntityComponentId* dependency =
        &g_array_index(component->dependencies, ShovelerEntityComponentId, i);
    ShovelerComponent* targetComponent = component->worldAdapter->getComponent(
        component,
        dependency->entityId,
        dependency->componentTypeId,
        component->worldAdapter->userData);
    if (targetComponent == NULL) {
      return false;
    }

    if (!shovelerComponentIsActive(targetComponent)) {
      return false;
    }
  }

  return true;
}

static long long int toDependencyTargetEntityId(
    ShovelerComponent* component, long long int entityIdValue) {
  if (entityIdValue != 0) {
    return entityIdValue;
  } else {
    return component->entityId;
  }
}
