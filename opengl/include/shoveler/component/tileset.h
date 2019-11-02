#ifndef SHOVELER_COMPONENT_TILESET_H
#define SHOVELER_COMPONENT_TILESET_H

#include <shoveler/types.h>

typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameTileset = "tileset";
static const char *shovelerComponentTilesetOptionKeyImageResource = "image_resource";
static const char *shovelerComponentTilesetOptionKeyNumColumns = "num_columns";
static const char *shovelerComponentTilesetOptionKeyNumRows = "num_rows";
static const char *shovelerComponentTilesetOptionKeyPadding = "padding";

ShovelerComponentType *shovelerComponentCreateTilesetType();
ShovelerTileset *shovelerComponentGetTileset(ShovelerComponent *component);

#endif
