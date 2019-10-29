#ifndef SHOVELER_COMPONENT_TILESET_H
#define SHOVELER_COMPONENT_TILESET_H

#include <shoveler/types.h>

typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewTilesetComponentTypeName = "tileset";
static const char *shovelerViewTilesetImageResourceOptionKey = "image_resource";
static const char *shovelerViewTilesetNumColumnsOptionKey = "num_columns";
static const char *shovelerViewTilesetNumRowsOptionKey = "num_rows";
static const char *shovelerViewTilesetPaddingOptionKey = "padding";

ShovelerComponentType *shovelerComponentCreateTilesetType();
ShovelerTileset *shovelerComponentGetTileset(ShovelerComponent *component);

#endif
