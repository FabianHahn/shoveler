#include "shoveler/component/drawable.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component_system.h"
#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/drawable/tiles.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateDrawableComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateDrawableComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddDrawableSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdDrawable);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateDrawableComponent;
  componentSystem->deactivateComponent = deactivateDrawableComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateDrawableComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponentDrawableType type =
      shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TYPE);
  switch (type) {
  case SHOVELER_COMPONENT_DRAWABLE_TYPE_CUBE:
    return shovelerDrawableCubeCreate();
  case SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD:
    return shovelerDrawableQuadCreate();
  case SHOVELER_COMPONENT_DRAWABLE_TYPE_POINT:
    return shovelerDrawablePointCreate();
  case SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES: {
    int tilesWidth = shovelerComponentGetFieldValueInt(
        component, SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TILES_WIDTH);
    int tilesHeight = shovelerComponentGetFieldValueInt(
        component, SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TILES_HEIGHT);
    return shovelerDrawableTilesCreate(tilesWidth, tilesHeight);
  }
  default:
    shovelerLogWarning("Failed to activate drawable with unknown type %d.", type);
    return NULL;
  }
}

static void deactivateDrawableComponent(ShovelerComponent* component, void* clientSystemPointer) {
  shovelerDrawableFree(component->systemData);
}
