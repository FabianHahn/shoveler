#ifndef SHOVELER_VIEW_TILESET_H
#define SHOVELER_VIEW_TILESET_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/material/tilemap.h>
#include <shoveler/view.h>

typedef struct {
	long long int textureEntityId;
	int columns;
	int rows;
} ShovelerViewTilesetConfiguration;

static const char *shovelerViewTilesetComponentName = "tileset";

bool shovelerViewEntityAddTileset(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration configuration);
ShovelerMaterialTilemapTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity);
const ShovelerViewTilesetConfiguration *shovelerViewEntityGetTilesetConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateTileset(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration configuration);
bool shovelerViewEntityRemoveTileset(ShovelerViewEntity *entity);

#endif
