#ifndef SHOVELER_COMPONENT_TILEMAP_TILES_H
#define SHOVELER_COMPONENT_TILEMAP_TILES_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerTextureStruct ShovelerTexture;

void shovelerClientSystemAddTilemapTilesSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerTexture* shovelerComponentGetTilemapTiles(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTilemapTiles);
  return component->systemData;
}

#endif
