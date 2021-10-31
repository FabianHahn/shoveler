#include "shoveler/component/tileset.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/image.h"
#include "shoveler/component_system.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"
#include "shoveler/tileset.h"

static void* activateTilesetComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTilesetComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTilesetSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTileset);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTilesetComponent;
  componentSystem->deactivateComponent = deactivateTilesetComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTilesetComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* imageComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILESET_FIELD_ID_IMAGE);
  assert(imageComponent != NULL);
  ShovelerImage* image = shovelerComponentGetImage(imageComponent);
  assert(image != NULL);

  int numColumns = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_COLUMNS);
  int numRows = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_ROWS);
  int padding = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_PADDING);
  ShovelerTileset* tileset = shovelerTilesetCreate(image, numColumns, numRows, padding);

  return tileset;
}

static void deactivateTilesetComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerTileset* tileset = (ShovelerTileset*) component->systemData;

  shovelerTilesetFree(tileset);
}
