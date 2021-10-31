#include "shoveler/component/sprite.h"

#include <assert.h>

#include "shoveler/canvas.h"
#include "shoveler/client_system.h"
#include "shoveler/component/canvas.h"
#include "shoveler/component/position.h"
#include "shoveler/component/text_sprite.h"
#include "shoveler/component/texture_sprite.h"
#include "shoveler/component/tile_sprite.h"
#include "shoveler/component/tilemap_sprite.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/sprite.h"
#include "shoveler/system.h"

static void* activateSpriteComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateSpriteComponent(ShovelerComponent* component, void* clientSystemPointer);
static bool liveUpdateSpritePositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer);
static ShovelerVector2 getSpritePosition(ShovelerComponent* component);

void shovelerClientSystemAddSpriteSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdSprite);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateSpriteComponent;
  componentSystem->deactivateComponent = deactivateSpriteComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION]
      .liveUpdateDependencyField = liveUpdateSpritePositionDependency;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateSpriteComponent(ShovelerComponent* component, void* clientSystemPointer) {
  bool hasTextSprite = shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TEXT_SPRITE);
  bool hasTileSprite = shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILE_SPRITE);
  bool hasTilemapSprite = shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILEMAP_SPRITE);
  bool hasTextureSprite = shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TEXTURE_SPRITE);
  int numDependencies = (hasTextSprite ? 1 : 0) + (hasTileSprite ? 1 : 0) +
      (hasTilemapSprite ? 1 : 0) + (hasTextureSprite ? 1 : 0);

  if (numDependencies != 1) {
    shovelerLogWarning(
        "Failed to activate canvas component of entity %lld: Exactly one dependency option must be "
        "set, but found %d set options.",
        component->entityId,
        numDependencies);
    return NULL;
  }

  ShovelerSprite* sprite = NULL;
  if (hasTextSprite) {
    ShovelerComponent* textSpriteComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TEXT_SPRITE);
    assert(textSpriteComponent != NULL);
    sprite = shovelerComponentGetTextSprite(textSpriteComponent);
  }

  if (hasTileSprite) {
    ShovelerComponent* tileSpriteComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILE_SPRITE);
    assert(tileSpriteComponent != NULL);
    sprite = shovelerComponentGetTileSprite(tileSpriteComponent);
  }

  if (hasTilemapSprite) {
    ShovelerComponent* tilemapSpriteComponent = shovelerComponentGetDependency(
        component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILEMAP_SPRITE);
    assert(tilemapSpriteComponent != NULL);
    sprite = shovelerComponentGetTilemapSprite(tilemapSpriteComponent);
  }

  if (hasTextureSprite) {
    ShovelerComponent* textureSpriteComponent = shovelerComponentGetDependency(
        component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TEXTURE_SPRITE);
    assert(textureSpriteComponent != NULL);
    sprite = shovelerComponentGetTextureSprite(textureSpriteComponent);
  }

  assert(sprite != NULL);

  if (shovelerComponentHasFieldValue(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION) &&
      shovelerComponentHasFieldValue(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X) &&
      shovelerComponentHasFieldValue(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y)) {
    shovelerSpriteUpdatePosition(sprite, getSpritePosition(component));
  }

  if (shovelerComponentHasFieldValue(component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE)) {
    shovelerSpriteUpdateSize(
        sprite,
        shovelerComponentGetFieldValueVector2(
            component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE));
  }

  shovelerSpriteSetEnableCollider(
      sprite,
      shovelerComponentGetFieldValueBool(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_ENABLE_COLLIDER));

  ShovelerComponent* canvasComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS);
  assert(canvasComponent != NULL);
  ShovelerCanvas* canvas = shovelerComponentGetCanvas(canvasComponent);

  int layerId = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER);
  shovelerCanvasAddSprite(canvas, layerId, sprite);

  return sprite;
}

static void deactivateSpriteComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerSprite* sprite = (ShovelerSprite*) component->systemData;
  assert(sprite != NULL);

  ShovelerComponent* canvasComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS);
  assert(canvasComponent != NULL);
  ShovelerCanvas* canvas = shovelerComponentGetCanvas(canvasComponent);

  int layerId = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER);
  shovelerCanvasRemoveSprite(canvas, layerId, sprite);
}

static bool liveUpdateSpritePositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer) {
  ShovelerSprite* sprite = (ShovelerSprite*) component->systemData;
  assert(sprite != NULL);

  if (shovelerComponentHasFieldValue(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION) &&
      shovelerComponentHasFieldValue(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X) &&
      shovelerComponentHasFieldValue(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y)) {
    shovelerSpriteUpdatePosition(sprite, getSpritePosition(component));
  }

  return false; // don't propagate
}

static ShovelerVector2 getSpritePosition(ShovelerComponent* component) {
  assert(shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION));
  assert(shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X));
  assert(shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y));

  ShovelerComponent* positionComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION);
  assert(positionComponent != NULL);
  const ShovelerVector3* position = shovelerComponentGetPosition(positionComponent);

  ShovelerCoordinateMapping positionMappingX =
      (ShovelerCoordinateMapping) shovelerComponentGetFieldValueInt(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X);
  ShovelerCoordinateMapping positionMappingY =
      (ShovelerCoordinateMapping) shovelerComponentGetFieldValueInt(
          component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y);

  return shovelerVector2(
      shovelerCoordinateMap(*position, positionMappingX),
      shovelerCoordinateMap(*position, positionMappingY));
}
