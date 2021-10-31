#ifndef SHOVELER_COMPONENT_TILE_SPRITE_H
#define SHOVELER_COMPONENT_TILE_SPRITE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerSpriteStruct ShovelerSprite;

void shovelerClientSystemAddTileSpriteSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerSprite* shovelerComponentGetTileSprite(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTileSprite);
  return component->systemData;
}

#endif
