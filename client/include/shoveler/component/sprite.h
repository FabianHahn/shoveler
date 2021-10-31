#ifndef SHOVELER_COMPONENT_SPRITE_H
#define SHOVELER_COMPONENT_SPRITE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerSpriteStruct ShovelerSprite;

void shovelerClientSystemAddSpriteSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerSprite* shovelerComponentGetSprite(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdSprite);
  return component->systemData;
}

#endif
