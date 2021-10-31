#ifndef SHOVELER_COMPONENT_FONT_H
#define SHOVELER_COMPONENT_FONT_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/base.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerFontStruct ShovelerFont;

void shovelerClientSystemAddFontSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerFont* shovelerComponentGetFont(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdFont);
  return component->systemData;
}

#endif
