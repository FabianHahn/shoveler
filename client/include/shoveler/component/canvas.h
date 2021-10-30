#ifndef SHOVELER_COMPONENT_CANVAS_H
#define SHOVELER_COMPONENT_CANVAS_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerCanvasStruct ShovelerCanvas;

void shovelerClientSystemAddCanvasSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerCanvas* shovelerComponentGetCanvas(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdCanvas);
  return component->systemData;
}

#endif
