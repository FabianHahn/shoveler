#ifndef SHOVELER_COMPONENT_TILEMAP_TILES_H
#define SHOVELER_COMPONENT_TILEMAP_TILES_H

typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewTilemapTilesComponentTypeName = "tilemap_tiles";
static const char *shovelerViewTilemapTilesImageResourceOptionKey = "image_resource";
static const char *shovelerViewTilemapTilesNumColumnsOptionKey = "num_columns";
static const char *shovelerViewTilemapTilesNumRowsOptionKey = "num_rows";
static const char *shovelerViewTilemapTilesTilesetColumnsOptionKey = "tileset_columns";
static const char *shovelerViewTilemapTilesTilesetRowsOptionKey = "tileset_rows";
static const char *shovelerViewTilemapTilesTilesetIdsOptionKey = "tileset_ids";

ShovelerComponentType *shovelerComponentCreateTilemapTilesType();
ShovelerTexture *shovelerComponentGetTilemapTiles(ShovelerComponent *component);

#endif
