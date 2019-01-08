#ifndef SHOVELER_VIEW_TILEMAP_TILE_SPRITE_H
#define SHOVELER_VIEW_TILEMAP_TILE_SPRITE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/view/position.h>
#include <shoveler/canvas.h>
#include <shoveler/view.h>

typedef enum {
	SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_X,
	SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_NEGATIVE_X,
	SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Y,
	SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_NEGATIVE_Y,
	SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Z,
	SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_NEGATIVE_Z,
} ShovelerViewTileSpriteCoordinateMappingConfiguration;

typedef struct {
	long long int tilesetEntityId;
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	ShovelerViewTileSpriteCoordinateMappingConfiguration positionMappingX;
	ShovelerViewTileSpriteCoordinateMappingConfiguration positionMappingY;
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

double shovelerViewPositionMapTileSpriteCoordinate(const ShovelerViewPosition *position, ShovelerViewTileSpriteCoordinateMappingConfiguration mapping);

#endif
