#ifndef SHOVELER_COMPONENT_DRAWABLE_H
#define SHOVELER_COMPONENT_DRAWABLE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerDrawableStruct ShovelerDrawable;

void shovelerClientSystemAddDrawableSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerDrawable* shovelerComponentGetDrawable(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdDrawable);
  return component->systemData;
}

#endif
