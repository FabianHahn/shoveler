#ifndef SHOVELER_VIEW_TILEMAP_TILES_H
#define SHOVELER_VIEW_TILEMAP_TILES_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/texture.h>
#include <shoveler/view.h>

typedef struct {
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	unsigned char tilesetId;
} ShovelerViewTilemapTilesTileConfiguration;

typedef struct {
	/**
	 * If set, tiles are defined by image resource component on the specified entity.
	 * Otherwise, uses inline tiles definition below.
	 */
	bool isImageResourceEntityDefinition;
	long long int imageResourceEntityId;
	int numColumns;
	int numRows;
	/** Array of tiles, where tile (column, row) is at position [row * numColumns + column] */
	ShovelerViewTilemapTilesTileConfiguration *tiles;
} ShovelerViewTilemapTilesConfiguration;

static const char *shovelerViewTilemapTilesComponentName = "tilemap_tiles";

/** Adds a tilemap tiles component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration);
ShovelerTexture *shovelerViewEntityGetTilemapTiles(ShovelerViewEntity *entity);
const ShovelerViewTilemapTilesConfiguration *shovelerViewEntityGetTilemapTilesConfiguration(ShovelerViewEntity *entity);
/** Updates a tilemap tiles component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration);
bool shovelerViewEntityUpdateTilemapTilesData(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesTileConfiguration *tiles);
bool shovelerViewEntityRemoveTilemapTiles(ShovelerViewEntity *entity);

#endif
