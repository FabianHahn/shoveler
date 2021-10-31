#ifndef SHOVELER_COMPONENT_TEXT_SPRITE_H
#define SHOVELER_COMPONENT_TEXT_SPRITE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerSpriteStruct ShovelerSprite;

void shovelerClientSystemAddTextSpriteSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerSprite* shovelerComponentGetTextSprite(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTextSprite);
  return component->systemData;
}

#endif
