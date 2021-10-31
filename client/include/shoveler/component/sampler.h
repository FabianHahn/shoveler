#ifndef SHOVELER_COMPONENT_SAMPLER_H
#define SHOVELER_COMPONENT_SAMPLER_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerSamplerStruct ShovelerSampler;

void shovelerClientSystemAddSamplerSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerSampler* shovelerComponentGetSampler(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdSampler);
  return component->systemData;
}

#endif
