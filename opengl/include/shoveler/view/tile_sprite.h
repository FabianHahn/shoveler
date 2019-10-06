#ifndef SHOVELER_VIEW_TILE_SPRITE_H
#define SHOVELER_VIEW_TILE_SPRITE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/canvas.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	long long int positionEntityId;
	long long int tilesetEntityId;
	int tilesetColumn;
	int tilesetRow;
	ShovelerCoordinateMapping positionMappingX;
	ShovelerCoordinateMapping positionMappingY;
	ShovelerVector2 size;
} ShovelerViewTileSpriteConfiguration;

static const char *shovelerViewTileSpriteComponentTypeName = "tile_sprite";
static const char *shovelerViewTileSpritePositionOptionKey = "position";
static const char *shovelerViewTileSpriteTilesetOptionKey = "tileset";
static const char *shovelerViewTileSpriteTilesetColumnOptionKey = "tileset_column";
static const char *shovelerViewTileSpriteTilesetRowOptionKey = "tileset_row";
static const char *shovelerViewTileSpritePositionMappingXOptionKey = "position_mapping_x";
static const char *shovelerViewTileSpritePositionMappingYOptionKey = "position_mapping_y";
static const char *shovelerViewTileSpriteSizeOptionKey = "size";

/** Adds a tile sprite component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration);
ShovelerCanvasTileSprite *shovelerViewEntityGetTileSprite(ShovelerViewEntity *entity);
/** Returns the current tile sprite configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetTileSpriteConfiguration(ShovelerViewEntity *entity, ShovelerViewTileSpriteConfiguration *outputConfiguration);
/** Updates a tile sprite component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration);
bool shovelerViewEntityRemoveTileSprite(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetTileSpriteComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentTypeName);
}

#endif
