#ifndef SHOVELER_COMPONENT_TILEMAP_H
#define SHOVELER_COMPONENT_TILEMAP_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerTilemapStruct ShovelerTilemap;

void shovelerClientSystemAddTilemapSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerTilemap* shovelerComponentGetTilemap(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTilemap);
  return component->systemData;
}

#endif
