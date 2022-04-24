#ifndef SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_H
#define SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerFontAtlasTextureStruct ShovelerFontAtlasTexture;

void shovelerClientSystemAddFontAtlasTextureSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerFontAtlasTexture* shovelerComponentGetFontAtlasTexture(
    ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdFontAtlasTexture);
  return component->systemData;
}

#endif
