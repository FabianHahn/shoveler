#ifndef SHOVELER_COMPONENT_TILEMAP_H
#define SHOVELER_COMPONENT_TILEMAP_H

typedef struct ShovelerTilemapStruct ShovelerTilemap; // forward declaration: tilemap.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdTilemap;

typedef enum {
	SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES,
	SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS,
	SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS,
} ShovelerComponentTilemapOptionId;

ShovelerComponentType *shovelerComponentCreateTilemapType();
ShovelerTilemap *shovelerComponentGetTilemap(ShovelerComponent *component);

#endif
