#ifndef SHOVELER_COMPONENT_TEXTURE_H
#define SHOVELER_COMPONENT_TEXTURE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerTextureStruct ShovelerTexture;

void shovelerClientSystemAddTextureSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerTexture* shovelerComponentGetTexture(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTexture);
  return component->systemData;
}

#endif
