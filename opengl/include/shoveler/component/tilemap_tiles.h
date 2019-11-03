#ifndef SHOVELER_COMPONENT_TILEMAP_TILES_H
#define SHOVELER_COMPONENT_TILEMAP_TILES_H

typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdTilemapTiles;

typedef enum {
	SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE_RESOURCE,
	SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS,
	SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS,
	SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS,
	SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS,
	SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS,
} ShovelerComponentTilemapTilesOptionId;

ShovelerComponentType *shovelerComponentCreateTilemapTilesType();
ShovelerTexture *shovelerComponentGetTilemapTiles(ShovelerComponent *component);

#endif
