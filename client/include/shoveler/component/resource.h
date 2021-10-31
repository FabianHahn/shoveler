#ifndef SHOVELER_COMPONENT_RESOURCE_H
#define SHOVELER_COMPONENT_RESOURCE_H

#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/base.h>

static inline void shovelerComponentGetResource(
    ShovelerComponent* component, const unsigned char** outputData, int* outputSize) {
  assert(component->type->id == shovelerComponentTypeIdResource);
  shovelerComponentGetFieldValueBytes(
      component, SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER, outputData, outputSize);
}

#endif
