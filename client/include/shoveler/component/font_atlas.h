#ifndef SHOVELER_COMPONENT_FONT_ATLAS_H
#define SHOVELER_COMPONENT_FONT_ATLAS_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/base.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerFontAtlasStruct ShovelerFontAtlas;

void shovelerClientSystemAddFontAtlasSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerFontAtlas* shovelerComponentGetFontAtlas(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdFontAtlas);
  return component->systemData;
}

#endif
