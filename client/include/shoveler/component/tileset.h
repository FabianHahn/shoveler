#ifndef SHOVELER_COMPONENT_TILESET_H
#define SHOVELER_COMPONENT_TILESET_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerTilesetStruct ShovelerTileset;

void shovelerClientSystemAddTilesetSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerTileset* shovelerComponentGetTileset(ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTileset);
  return component->systemData;
}

#endif
