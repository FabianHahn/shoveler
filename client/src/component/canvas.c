#include "shoveler/component/canvas.h"

#include <assert.h>

#include "shoveler/canvas.h"
#include "shoveler/client_system.h"
#include "shoveler/colliders.h"
#include "shoveler/component/resource.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateCanvasComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateCanvasComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddCanvasSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdCanvas);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateCanvasComponent;
  componentSystem->deactivateComponent = deactivateCanvasComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateCanvasComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  int numLayers =
      shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_CANVAS_FIELD_ID_NUM_LAYERS);
  if (numLayers < 1) {
    shovelerLogWarning(
        "Failed to activate canvas component on entity %lld: num_layers option must be positive, "
        "but got %d.",
        component->entityId,
        numLayers);
    return NULL;
  }

  ShovelerCanvas* canvas = shovelerCanvasCreate(numLayers);
  shovelerCollidersAddCollider2(clientSystem->colliders, &canvas->collider);

  return canvas;
}

static void deactivateCanvasComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;
  ShovelerCanvas* canvas = (ShovelerCanvas*) component->systemData;

  shovelerCollidersRemoveCollider2(clientSystem->colliders, &canvas->collider);
  shovelerCanvasFree(canvas);
}
