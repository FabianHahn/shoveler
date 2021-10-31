#include "shoveler/component/tilemap_sprite.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/material.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/sprite/tilemap.h"
#include "shoveler/system.h"

static void* activateTilemapSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTilemapSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTilemapSpriteSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTilemapSprite);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTilemapSpriteComponent;
  componentSystem->deactivateComponent = deactivateTilemapSpriteComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTilemapSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* materialComponent = shovelerComponentGetDependency(
      component, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_MATERIAL);
  assert(materialComponent != NULL);
  ShovelerMaterial* material = shovelerComponentGetMaterial(materialComponent);
  assert(material != NULL);

  ShovelerComponent* tilemapComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_TILEMAP);
  assert(tilemapComponent != NULL);
  ShovelerTilemap* tilemap = shovelerComponentGetTilemap(tilemapComponent);
  assert(tilemap != NULL);

  ShovelerSprite* tileSprite = shovelerSpriteTilemapCreate(material, tilemap);
  return tileSprite;
}

static void deactivateTilemapSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerSprite* tileSprite = (ShovelerSprite*) component->systemData;
  assert(tileSprite != NULL);

  shovelerSpriteFree(tileSprite);
}
