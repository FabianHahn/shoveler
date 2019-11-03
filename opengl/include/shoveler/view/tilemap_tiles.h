#ifndef SHOVELER_VIEW_TILEMAP_TILES_H
#define SHOVELER_VIEW_TILEMAP_TILES_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/component/tilemap_tiles.h>
#include <shoveler/texture.h>
#include <shoveler/view.h>

typedef struct {
	/**
	 * If set, tiles are defined by image resource component on the specified entity.
	 * Otherwise, uses inline tiles definition below.
	 */
	bool isImageResourceEntityDefinition;
	long long int imageResourceEntityId;
	int numColumns;
	int numRows;
	/** Array of tileset column bytes, where tile (column, row) is at position [row * numColumns + column] */
	const unsigned char *tilesetColumns;
	/** Array of tileset row bytes, where tile (column, row) is at position [row * numColumns + column] */
	const unsigned char *tilesetRows;
	/** Array of tileset id bytes, where tile (column, row) is at position [row * numColumns + column] */
	const unsigned char *tilesetIds;
} ShovelerViewTilemapTilesConfiguration;

/** Adds a tilemap tiles component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration);
ShovelerTexture *shovelerViewEntityGetTilemapTiles(ShovelerViewEntity *entity);
/** Returns the current tilemap tiles configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetTilemapTilesConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapTilesConfiguration *outputConfiguration);
/** Updates a tilemap tiles component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration);
bool shovelerViewEntityRemoveTilemapTiles(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetTilemapTilesComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapTiles);
}

#endif
