#include "shoveler/component/tilemap.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/tilemap_colliders.h"
#include "shoveler/component/tilemap_tiles.h"
#include "shoveler/component/tileset.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"
#include "shoveler/texture.h"
#include "shoveler/tilemap.h"

static void* activateTilemapComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTilemapComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTilemapSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTilemap);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTilemapComponent;
  componentSystem->deactivateComponent = deactivateTilemapComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTilemapComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* tilesComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILES);
  assert(tilesComponent != NULL);
  ShovelerTexture* tiles = shovelerComponentGetTilemapTiles(tilesComponent);
  assert(tiles != NULL);
  int numTilesColumns = tiles->width;
  int numTilesRows = tiles->height;

  ShovelerComponent* collidersComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_COLLIDERS);
  assert(collidersComponent != NULL);
  const bool* colliders = shovelerComponentGetTilemapColliders(collidersComponent);
  assert(colliders != NULL);
  int numCollidersColumns = shovelerComponentGetFieldValueInt(
      collidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS);
  int numCollidersRows = shovelerComponentGetFieldValueInt(
      collidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS);

  if (numTilesColumns != numCollidersColumns || numTilesRows != numCollidersRows) {
    shovelerLogWarning(
        "Failed to activate tilemap %lld because dependency tiles dimensions don't match "
        "dependency colliders dimensions.",
        component->entityId);
    return NULL;
  }

  ShovelerTilemap* tilemap = shovelerTilemapCreate(tiles, colliders);

  const ShovelerComponentFieldValue* tilesetsValue = shovelerComponentGetFieldValue(
      component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILESETS);
  assert(tilesetsValue != NULL);

  for (int i = 0; i < tilesetsValue->entityIdArrayValue.size; i++) {
    ShovelerComponent* tilesetComponent = shovelerComponentGetArrayDependency(
        component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILESETS, i);
    ShovelerTileset* tileset = shovelerComponentGetTileset(tilesetComponent);
    assert(tileset != NULL);

    shovelerTilemapAddTileset(tilemap, tileset);
  }

  return tilemap;
}

static void deactivateTilemapComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerTilemap* tilemap = (ShovelerTilemap*) component->systemData;

  shovelerTilemapFree(tilemap);
}
