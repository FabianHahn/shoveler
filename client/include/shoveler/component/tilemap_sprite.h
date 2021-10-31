#ifndef SHOVELER_COMPONENT_TILEMAP_SPRITE_H
#define SHOVELER_COMPONENT_TILEMAP_SPRITE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerSpriteStruct ShovelerSprite;

void shovelerClientSystemAddTilemapSpriteSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerSprite* shovelerComponentGetTilemapSprite(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTilemapSprite);
  return component->systemData;
}

#endif
