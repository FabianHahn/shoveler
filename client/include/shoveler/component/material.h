#ifndef SHOVELER_COMPONENT_MATERIAL_H
#define SHOVELER_COMPONENT_MATERIAL_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerMaterialStruct ShovelerMaterial;

void shovelerClientSystemAddMaterialSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerMaterial* shovelerComponentGetMaterial(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdMaterial);
  return component->systemData;
}

#endif
