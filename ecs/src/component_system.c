#include "shoveler/component_system.h"

#include <stdlib.h> // malloc free

#include "shoveler/component.h"
#include "shoveler/component_type.h"

static bool requiresAuthority(ShovelerComponent* component, void* userData);
static bool canLiveUpdateField(
    ShovelerComponent* component, int fieldId, const ShovelerComponentField* field, void* userData);
static bool canLiveUpdateDependencyField(
    ShovelerComponent* component, int fieldId, const ShovelerComponentField* field, void* userData);
static bool liveUpdateField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData);
static bool liveUpdateDependencyField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* userData);
static void* activateComponent(ShovelerComponent* component, void* userData);
static bool updateComponent(ShovelerComponent* component, double dt, void* userData);
static void deactivateComponent(ShovelerComponent* component, void* userData);

ShovelerComponentSystem* shovelerComponentSystemCreate(ShovelerComponentType* componentType) {
  ShovelerComponentSystem* componentSystem = malloc(sizeof(ShovelerComponentSystem));
  componentSystem->componentAdapter = malloc(sizeof(ShovelerComponentSystemAdapter));
  componentSystem->componentAdapter->requiresAuthority = requiresAuthority;
  componentSystem->componentAdapter->canLiveUpdateField = canLiveUpdateField;
  componentSystem->componentAdapter->canLiveUpdateDependencyField = canLiveUpdateDependencyField;
  componentSystem->componentAdapter->liveUpdateField = liveUpdateField;
  componentSystem->componentAdapter->liveUpdateDependencyField = liveUpdateDependencyField;
  componentSystem->componentAdapter->activateComponent = activateComponent;
  componentSystem->componentAdapter->updateComponent = updateComponent;
  componentSystem->componentAdapter->deactivateComponent = deactivateComponent;
  componentSystem->componentAdapter->userData = componentSystem;
  componentSystem->componentType = componentType;
  componentSystem->requiresAuthority = false;
  componentSystem->fieldOptions =
      malloc(componentType->numFields * sizeof(ShovelerComponentSystemFieldOptions));
  for (int i = 0; i < componentType->numFields; i++) {
    componentSystem->fieldOptions[i].liveUpdateField = NULL;
    componentSystem->fieldOptions[i].liveUpdateDependencyField = NULL;
  }
  componentSystem->activateComponent = NULL;
  componentSystem->updateComponent = NULL;
  componentSystem->deactivateComponent = NULL;

  return componentSystem;
}

void shovelerComponentSystemFree(ShovelerComponentSystem* componentSystem) {
  free(componentSystem->fieldOptions);
  free(componentSystem->componentAdapter);
  free(componentSystem);
}

static bool requiresAuthority(ShovelerComponent* component, void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;
  return componentSystem->requiresAuthority;
}

static bool canLiveUpdateField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;
  assert(component->type == componentSystem->componentType);
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);
  return componentSystem->fieldOptions[fieldId].liveUpdateField != NULL;
}

static bool canLiveUpdateDependencyField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;
  assert(component->type == componentSystem->componentType);
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);
  return componentSystem->fieldOptions[fieldId].liveUpdateDependencyField != NULL;
}

static bool liveUpdateField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;
  assert(component->type == componentSystem->componentType);
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);

  ShovelerComponentSystemFieldOptions* fieldOptions = &componentSystem->fieldOptions[fieldId];
  assert(fieldOptions->liveUpdateField != NULL);
  return fieldOptions->liveUpdateField(
      component, fieldId, field, fieldValue, componentSystem->callbackUserData);
}

static bool liveUpdateDependencyField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;
  assert(component->type == componentSystem->componentType);
  assert(fieldId >= 0);
  assert(fieldId < component->type->numFields);

  ShovelerComponentSystemFieldOptions* fieldOptions = &componentSystem->fieldOptions[fieldId];
  assert(fieldOptions->liveUpdateDependencyField != NULL);
  return fieldOptions->liveUpdateDependencyField(
      component, fieldId, field, dependencyComponent, componentSystem->callbackUserData);
}

static void* activateComponent(ShovelerComponent* component, void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;

  if (componentSystem->activateComponent != NULL) {
    return componentSystem->activateComponent(component, componentSystem->callbackUserData);
  }

  return component;
}

static bool updateComponent(ShovelerComponent* component, double dt, void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;

  if (componentSystem->updateComponent != NULL) {
    return componentSystem->updateComponent(component, dt, componentSystem->callbackUserData);
  }

  return false;
}

static void deactivateComponent(ShovelerComponent* component, void* componentSystemPointer) {
  ShovelerComponentSystem* componentSystem = componentSystemPointer;

  if (componentSystem->deactivateComponent != NULL) {
    componentSystem->deactivateComponent(component, componentSystem->callbackUserData);
  }
}
