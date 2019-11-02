#ifndef SHOVELER_VIEW_TILESET_H
#define SHOVELER_VIEW_TILESET_H

#include <stdbool.h> // bool

#include <shoveler/component/tileset.h>
#include <shoveler/view.h>

typedef struct {
	long long int imageResourceEntityId;
	int numColumns;
	int numRows;
	int padding;
} ShovelerViewTilesetConfiguration;

/** Adds a tileset component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration);
ShovelerTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity);
/** Returns the current tileset configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetTilesetConfiguration(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration *outputConfiguration);
/** Updates a tileset component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration);
bool shovelerViewEntityRemoveTileset(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetTilesetComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileset);
}

#endif
