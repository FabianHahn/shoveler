#ifndef SHOVELER_COMPONENT_LIGHT_H
#define SHOVELER_COMPONENT_LIGHT_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerLightStruct ShovelerLight;

void shovelerClientSystemAddLightSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerLight* shovelerComponentGetLight(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdLight);
  return component->systemData;
}

#endif
