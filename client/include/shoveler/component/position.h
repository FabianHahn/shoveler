#ifndef SHOVELER_COMPONENT_POSITION_H
#define SHOVELER_COMPONENT_POSITION_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/base.h>
#include <shoveler/types.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;

void shovelerClientSystemAddPositionSystem(ShovelerClientSystem* clientSystem);

static inline const ShovelerVector3* shovelerComponentGetPosition(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdPosition);
  return component->systemData;
}

#endif
