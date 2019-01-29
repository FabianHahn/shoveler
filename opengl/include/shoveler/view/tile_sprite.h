#ifndef SHOVELER_VIEW_TILEMAP_TILE_SPRITE_H
#define SHOVELER_VIEW_TILEMAP_TILE_SPRITE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/view/position.h>
#include <shoveler/canvas.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	long long int tilesetEntityId;
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	ShovelerCoordinateMapping positionMappingX;
	ShovelerCoordinateMapping positionMappingY;
	ShovelerVector2 size;
} ShovelerViewTileSpriteConfiguration;

static const char *shovelerViewTileSpriteComponentName = "tile_sprite";

/** Adds a canvas tile sprite component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration);
ShovelerCanvasTileSprite *shovelerViewEntityGetTileSprite(ShovelerViewEntity *entity);
const ShovelerViewTileSpriteConfiguration *shovelerViewEntityGetTileSpriteConfiguration(ShovelerViewEntity *entity);
/** Updates a canvas tile sprite component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration);
bool shovelerViewEntityRemoveTileSprite(ShovelerViewEntity *entity);

#endif
