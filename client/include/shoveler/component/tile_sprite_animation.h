#ifndef SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_H
#define SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerTileSpriteAnimationStruct ShovelerTileSpriteAnimation;

void shovelerClientSystemAddTileSpriteAnimationSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerTileSpriteAnimation* shovelerComponentGetTileSpriteAnimation(
    ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTileSpriteAnimation);
  return component->systemData;
}

#endif
