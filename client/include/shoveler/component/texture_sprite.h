#ifndef SHOVELER_COMPONENT_TEXTURE_SPRITE_H
#define SHOVELER_COMPONENT_TEXTURE_SPRITE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerSpriteStruct ShovelerSprite;

void shovelerClientSystemAddTextureSpriteSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerSprite* shovelerComponentGetTextureSprite(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTextureSprite);
  return component->systemData;
}

#endif
