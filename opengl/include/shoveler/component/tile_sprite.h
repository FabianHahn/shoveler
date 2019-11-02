#ifndef SHOVELER_COMPONENT_TILE_SPRITE_H
#define SHOVELER_COMPONENT_TILE_SPRITE_H

typedef struct ShovelerCanvasTileSpriteStruct ShovelerCanvasTileSprite; // forward declaration: tile_sprite.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameTileSprite = "tile_sprite";
static const char *shovelerComponentTileSpriteOptionKeyPosition = "position";
static const char *shovelerComponentTileSpriteOptionKeyTileset = "tileset";
static const char *shovelerComponentTileSpriteOptionKeyTilesetColumn = "tileset_column";
static const char *shovelerComponentTileSpriteOptionKeyTilesetRow = "tileset_row";
static const char *shovelerComponentTileSpriteOptionKeyPositionMappingX = "position_mapping_x";
static const char *shovelerComponentTileSpriteOptionKeyPositionMappingY = "position_mapping_y";
static const char *shovelerComponentTileSpriteOptionKeySize = "size";

ShovelerComponentType *shovelerComponentCreateTileSpriteType();
ShovelerCanvasTileSprite *shovelerComponentGetTileSprite(ShovelerComponent *component);

#endif
