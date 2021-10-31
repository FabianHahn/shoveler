#ifndef SHOVELER_COMPONENT_IMAGE_H
#define SHOVELER_COMPONENT_IMAGE_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/base.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerImageStruct ShovelerImage;

void shovelerClientSystemAddImageSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerImage* shovelerComponentGetImage(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdImage);
  return component->systemData;
}

#endif
