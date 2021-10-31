#include "shoveler/component/tile_sprite.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/material.h"
#include "shoveler/component/tileset.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/sprite/tile.h"
#include "shoveler/system.h"

static void* activateTileSpriteComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTileSpriteComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTileSpriteSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTileSprite);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTileSpriteComponent;
  componentSystem->deactivateComponent = deactivateTileSpriteComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTileSpriteComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* materialComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_MATERIAL);
  assert(materialComponent != NULL);
  ShovelerMaterial* material = shovelerComponentGetMaterial(materialComponent);
  assert(material != NULL);

  ShovelerComponent* tilesetComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET);
  assert(tilesetComponent != NULL);
  ShovelerTileset* tileset = shovelerComponentGetTileset(tilesetComponent);
  assert(tileset != NULL);

  int tilesetColumn = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_COLUMN);
  int tilesetRow = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_ROW);

  ShovelerSprite* tileSprite =
      shovelerSpriteTileCreate(material, tileset, tilesetColumn, tilesetRow);
  return tileSprite;
}

static void deactivateTileSpriteComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerSprite* tileSprite = (ShovelerSprite*) component->systemData;
  assert(tileSprite != NULL);

  shovelerSpriteFree(tileSprite);
}
