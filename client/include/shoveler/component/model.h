#ifndef SHOVELER_COMPONENT_MODEL_H
#define SHOVELER_COMPONENT_MODEL_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerModelStruct ShovelerModel;

void shovelerClientSystemAddModelSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerModel* shovelerComponentGetModel(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdModel);
  return component->systemData;
}

#endif
