#ifndef SHOVELER_COMPONENT_TILESET_H
#define SHOVELER_COMPONENT_TILESET_H

#include <shoveler/types.h>

typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdTileset;

typedef enum {
	SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE_RESOURCE,
	SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS,
	SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS,
	SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING,
} ShovelerComponentTilsetOptionId;

ShovelerComponentType *shovelerComponentCreateTilesetType();
ShovelerTileset *shovelerComponentGetTileset(ShovelerComponent *component);

#endif
