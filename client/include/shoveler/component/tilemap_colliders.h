#ifndef SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H
#define SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;

void shovelerClientSystemAddTilemapCollidersSystem(ShovelerClientSystem* clientSystem);

static inline const bool* shovelerComponentGetTilemapColliders(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTilemapColliders);
  return component->systemData;
}

#endif
