#ifndef SHOVELER_COMPONENT_TILEMAP_TILES_H
#define SHOVELER_COMPONENT_TILEMAP_TILES_H

typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameTilemapTiles = "tilemap_tiles";
static const char *shovelerComponentTilemapTilesOptionKeyImageResource = "image_resource";
static const char *shovelerComponentTilemapTilesOptionKeyNumColumns = "num_columns";
static const char *shovelerComponentTilemapTilesOptionKeyNumRows = "num_rows";
static const char *shovelerComponentTilemapTilesOptionKeyTilesetColumns = "tileset_columns";
static const char *shovelerComponentTilemapTilesOptionKeyTilesetRows = "tileset_rows";
static const char *shovelerComponentTilemapTilesOptionKeyTilesetIds = "tileset_ids";

ShovelerComponentType *shovelerComponentCreateTilemapTilesType();
ShovelerTexture *shovelerComponentGetTilemapTiles(ShovelerComponent *component);

#endif
