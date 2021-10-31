#include "shoveler/component/tile_sprite_animation.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/position.h"
#include "shoveler/component/tile_sprite.h"
#include "shoveler/component/tileset.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/sprite/tile.h"
#include "shoveler/system.h"
#include "shoveler/tile_sprite_animation.h"

static void* activateTileSpriteAnimationComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTileSpriteAnimationComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static bool liveUpdateTileSpriteAnimationPositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer);
static ShovelerVector2 getTileSpriteAnimationPosition(ShovelerComponent* component);

void shovelerClientSystemAddTileSpriteAnimationSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType = shovelerSchemaGetComponentType(
      clientSystem->schema, shovelerComponentTypeIdTileSpriteAnimation);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTileSpriteAnimationComponent;
  componentSystem->deactivateComponent = deactivateTileSpriteAnimationComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION]
      .liveUpdateDependencyField = liveUpdateTileSpriteAnimationPositionDependency;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTileSpriteAnimationComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* tileSpriteComponent = shovelerComponentGetDependency(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE);
  assert(tileSpriteComponent != NULL);
  ShovelerSprite* tileSprite = shovelerComponentGetTileSprite(tileSpriteComponent);
  assert(tileSprite != NULL);
  ShovelerTileset* tileset = shovelerSpriteTileGetTileset(tileSprite);
  assert(tileset != NULL);

  if (tileset->columns < 4 || tileset->rows < 3) {
    shovelerLogWarning(
        "Failed to activate tile sprite animation of entity %lld because the tileset of dependency "
        "tile sprite on entity %lld doesn't have enough columns and rows.",
        component->entityId,
        tileSpriteComponent->entityId);
    return false;
  }

  float moveAmountThreshold = shovelerComponentGetFieldValueFloat(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD);
  ShovelerTileSpriteAnimation* animation = shovelerTileSpriteAnimationCreate(
      tileSprite, getTileSpriteAnimationPosition(component), moveAmountThreshold);
  return animation;
}

static void deactivateTileSpriteAnimationComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerTileSpriteAnimation* animation = (ShovelerTileSpriteAnimation*) component->systemData;

  shovelerTileSpriteAnimationFree(animation);
}

static bool liveUpdateTileSpriteAnimationPositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer) {
  ShovelerTileSpriteAnimation* animation = (ShovelerTileSpriteAnimation*) component->systemData;
  assert(animation != NULL);

  shovelerTileSpriteAnimationUpdate(animation, getTileSpriteAnimationPosition(component));

  return false; // don't propagate
}

static ShovelerVector2 getTileSpriteAnimationPosition(ShovelerComponent* component) {
  ShovelerComponent* positionComponent = shovelerComponentGetDependency(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION);
  assert(positionComponent != NULL);
  const ShovelerVector3* position = shovelerComponentGetPosition(positionComponent);

  ShovelerCoordinateMapping positionMappingX = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X);
  ShovelerCoordinateMapping positionMappingY = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y);

  return shovelerVector2(
      shovelerCoordinateMap(*position, positionMappingX),
      shovelerCoordinateMap(*position, positionMappingY));
}
