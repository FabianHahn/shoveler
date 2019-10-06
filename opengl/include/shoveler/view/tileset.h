#ifndef SHOVELER_VIEW_TILESET_H
#define SHOVELER_VIEW_TILESET_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/tileset.h>
#include <shoveler/view.h>

typedef struct {
	long long int imageResourceEntityId;
	unsigned char columns;
	unsigned char rows;
	unsigned char padding;
} ShovelerViewTilesetConfiguration;

static const char *shovelerViewTilesetComponentTypeName = "tileset";

bool shovelerViewEntityAddTileset(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration configuration);
ShovelerTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity);
const ShovelerViewTilesetConfiguration *shovelerViewEntityGetTilesetConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateTileset(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration configuration);
bool shovelerViewEntityRemoveTileset(ShovelerViewEntity *entity);

#endif
