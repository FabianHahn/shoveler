#ifndef SHOVELER_VIEW_TILEMAP_LAYER_H
#define SHOVELER_VIEW_TILEMAP_LAYER_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/texture.h>
#include <shoveler/view.h>

typedef struct {
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	unsigned char tilesetId;
} ShovelerViewTilemapLayerTileConfiguration;

typedef struct {
	/**
	 * If set, layer is defined by image resource component on the specified entity.
	 * Otherwise, uses inline layer definition below.
	 */
	bool isImageResourceEntityDefinition;
	long long int imageResourceEntityId;
	int numColumns;
	int numRows;
	/** Array of tiles, where tile (column, row) is at position [row * numColumns + column] */
	ShovelerViewTilemapLayerTileConfiguration *tiles;
} ShovelerViewTilemapLayerConfiguration;

static const char *shovelerViewTilemapLayerComponentName = "tilemap_layer";

bool shovelerViewEntityAddTilemapLayer(ShovelerViewEntity *entity, ShovelerViewTilemapLayerConfiguration configuration);
ShovelerTexture *shovelerViewEntityGetTilemapLayer(ShovelerViewEntity *entity);
const ShovelerViewTilemapLayerConfiguration *shovelerViewEntityGetTilemapLayerConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateTilemapLayer(ShovelerViewEntity *entity, ShovelerViewTilemapLayerConfiguration configuration);
bool shovelerViewEntityRemoveTilemapLayer(ShovelerViewEntity *entity);

#endif
