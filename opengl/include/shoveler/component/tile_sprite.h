#ifndef SHOVELER_COMPONENT_TILE_SPRITE_H
#define SHOVELER_COMPONENT_TILE_SPRITE_H

typedef struct ShovelerCanvasTileSpriteStruct ShovelerCanvasTileSprite; // forward declaration: tile_sprite.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewTileSpriteComponentTypeName = "tile_sprite";
static const char *shovelerViewTileSpritePositionOptionKey = "position";
static const char *shovelerViewTileSpriteTilesetOptionKey = "tileset";
static const char *shovelerViewTileSpriteTilesetColumnOptionKey = "tileset_column";
static const char *shovelerViewTileSpriteTilesetRowOptionKey = "tileset_row";
static const char *shovelerViewTileSpritePositionMappingXOptionKey = "position_mapping_x";
static const char *shovelerViewTileSpritePositionMappingYOptionKey = "position_mapping_y";
static const char *shovelerViewTileSpriteSizeOptionKey = "size";

ShovelerComponentType *shovelerComponentCreateTileSpriteType();
ShovelerCanvasTileSprite *shovelerComponentGetTileSprite(ShovelerComponent *component);

#endif
