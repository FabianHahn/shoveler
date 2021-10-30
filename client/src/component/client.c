#include "shoveler/component/client.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/model.h"
#include "shoveler/component/position.h"
#include "shoveler/component_system.h"
#include "shoveler/controller.h"
#include "shoveler/model.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateClientComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateClientComponent(ShovelerComponent* component, void* clientSystemPointer);
static void moveController(
    ShovelerController* controller, ShovelerVector3 position, void* componentPointer);

void shovelerClientSystemAddClientSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdClient);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateClientComponent;
  componentSystem->deactivateComponent = deactivateClientComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_CLIENT_FIELD_ID_POSITION]
      .liveUpdateDependencyField = shovelerComponentSystemLiveUpdateDependencyFieldNoop;
  componentSystem->requiresAuthority = true;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateClientComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerComponent* positionComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_POSITION);
  assert(positionComponent != NULL);
  const ShovelerVector3* position = shovelerComponentGetPosition(positionComponent);

  ShovelerReferenceFrame frame = clientSystem->controller->frame;
  frame.position = *position;
  shovelerControllerSetFrame(clientSystem->controller, &frame);

  ShovelerControllerMoveCallback* moveCallback =
      shovelerControllerAddMoveCallback(clientSystem->controller, moveController, component);

  if (shovelerComponentHasFieldValue(component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_MODEL)) {
    ShovelerComponent* modelComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_MODEL);
    assert(modelComponent != NULL);
    ShovelerModel* model = shovelerComponentGetModel(modelComponent);
    assert(model != NULL);

    model->visible = false;
  }

  return moveCallback;
}

static void deactivateClientComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerControllerMoveCallback* moveCallback = component->systemData;
  shovelerControllerRemoveMoveCallback(clientSystem->controller, moveCallback);

  if (shovelerComponentHasFieldValue(component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_MODEL)) {
    ShovelerComponent* modelComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_MODEL);
    assert(modelComponent != NULL);
    ShovelerModel* model = shovelerComponentGetModel(modelComponent);
    assert(model != NULL);

    model->visible = true;
  }
}

static void moveController(
    ShovelerController* controller, ShovelerVector3 position, void* componentPointer) {
  ShovelerComponent* component = componentPointer;

  ShovelerComponent* positionComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_POSITION);
  assert(positionComponent != NULL);

  shovelerComponentUpdateFieldVector3(
      positionComponent, SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES, position);
}
