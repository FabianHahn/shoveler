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
	long long int *tilesetEntityIds;
} ShovelerViewTilemapConfiguration;

static const char *shovelerViewTilemapComponentTypeName = "tilemap";

/** Adds a tilemap component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration);
ShovelerTilemap *shovelerViewEntityGetTilemap(ShovelerViewEntity *entity);
const ShovelerViewTilemapConfiguration *shovelerViewEntityGetTilemapConfiguration(ShovelerViewEntity *entity);
/** Updates a tilemap component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTilemap(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration configuration);
bool shovelerViewEntityRemoveTilemap(ShovelerViewEntity *entity);

#endif
