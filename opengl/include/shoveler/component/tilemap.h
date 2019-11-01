#ifndef SHOVELER_COMPONENT_TILEMAP_H
#define SHOVELER_COMPONENT_TILEMAP_H

typedef struct ShovelerTilemapStruct ShovelerTilemap; // forward declaration: tilemap.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewTilemapComponentTypeName = "tilemap";
static const char *shovelerViewTilemapTilesOptionKey = "tiles";
static const char *shovelerViewTilemapCollidersOptionKey = "colliders";
static const char *shovelerViewTilemapTilesetsOptionKey = "tilesets";

ShovelerComponentType *shovelerComponentCreateTilemapType();
ShovelerTilemap *shovelerComponentGetTilemap(ShovelerComponent *component);

#endif
