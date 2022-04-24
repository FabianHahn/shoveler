#include "shoveler/component/position.h"

#include <assert.h>
#include <stdlib.h>

#include "shoveler/client_system.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activatePositionComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivatePositionComponent(ShovelerComponent* component, void* clientSystemPointer);
static bool liveUpdatePositionCoordinates(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* clientSystemPointer);
static bool liveUpdatePositionReleativeParent(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer);
static void updatePosition(ShovelerComponent* component);

void shovelerClientSystemAddPositionSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdPosition);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activatePositionComponent;
  componentSystem->deactivateComponent = deactivatePositionComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES].liveUpdateField =
      liveUpdatePositionCoordinates;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_POSITION_FIELD_ID_RELATIVE_PARENT_POSITION]
      .liveUpdateDependencyField = liveUpdatePositionReleativeParent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activatePositionComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponentPositionType type =
      shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE);
  bool hasRelativeParentPosition = shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_POSITION_FIELD_ID_RELATIVE_PARENT_POSITION);

  if (type == SHOVELER_COMPONENT_POSITION_TYPE_RELATIVE && !hasRelativeParentPosition) {
    shovelerLogWarning(
        "Failed to activate entity %lld position because its type is relative but it doesn't have "
        "a relative parent position set.",
        component->entityId);
    return NULL;
  }

  component->systemData = malloc(sizeof(ShovelerVector3));
  updatePosition(component);

  return component->systemData;
}

static void deactivatePositionComponent(ShovelerComponent* component, void* clientSystemPointer) {
  free(component->systemData);
}

static bool liveUpdatePositionCoordinates(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* clientSystemPointer) {
  updatePosition(component);

  return true; // propagate update
}

static bool liveUpdatePositionReleativeParent(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer) {
  updatePosition(component);

  return true; // propagate update
}

void updatePosition(ShovelerComponent* component) {
  ShovelerVector3* position = component->systemData;

  ShovelerComponentPositionType type =
      shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE);
  ShovelerVector3 coordinates = shovelerComponentGetFieldValueVector3(
      component, SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES);

  if (type == SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE) {
    *position = coordinates;
  } else {
    ShovelerComponent* relativeParentPositionComponent = shovelerComponentGetDependency(
        component, SHOVELER_COMPONENT_POSITION_FIELD_ID_RELATIVE_PARENT_POSITION);
    assert(relativeParentPositionComponent != NULL);
    const ShovelerVector3* parentPosition =
        shovelerComponentGetPosition(relativeParentPositionComponent);
    *position = shovelerVector3LinearCombination(1.0f, coordinates, 1.0f, *parentPosition);
  }
}
