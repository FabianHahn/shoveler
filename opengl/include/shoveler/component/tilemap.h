#ifndef SHOVELER_COMPONENT_TILEMAP_H
#define SHOVELER_COMPONENT_TILEMAP_H

typedef struct ShovelerTilemapStruct ShovelerTilemap; // forward declaration: tilemap.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdTilemap;
static const char *shovelerComponentTilemapOptionKeyTiles = "tiles";
static const char *shovelerComponentTilemapOptionKeyColliders = "colliders";
static const char *shovelerComponentTilemapOptionKeyTilesets = "tilesets";

ShovelerComponentType *shovelerComponentCreateTilemapType();
ShovelerTilemap *shovelerComponentGetTilemap(ShovelerComponent *component);

#endif
