#ifndef SHOVELER_VIEW_TILEMAP_H
#define SHOVELER_VIEW_TILEMAP_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/material/tilemap.h>
#include <shoveler/tilemap.h>
#include <shoveler/tileset.h>
#include <shoveler/view.h>

typedef struct {
	long long int tilesEntityId;
	int numTilesets;
	const long long int *tilesetEntityIds;
} ShovelerViewTilemapConfiguration;

static const char *shovelerViewTilemapComponentTypeName = "tilemap";
static const char *shovelerViewTilemapTilesOptionKey = "tiles";
static const char *shovelerViewTilemapTilesetsOptionKey = "tilesets";

/** Adds a tilemap component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration);
ShovelerTilemap *shovelerViewEntityGetTilemap(ShovelerViewEntity *entity);
/** Returns the current tilemap configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetTilemapConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration *outputConfiguration);
/** Updates a tilemap component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration);
bool shovelerViewEntityRemoveTilemap(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetTilemapComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentTypeName);
}

#endif
