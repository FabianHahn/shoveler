#ifndef SHOVELER_VIEW_TILEMAP_COLLIDERS_H
#define SHOVELER_VIEW_TILEMAP_COLLIDERS_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/component/tilemap_colliders.h>
#include <shoveler/texture.h>
#include <shoveler/view.h>

typedef struct {
	int numColumns;
	int numRows;
	/** Array of collider boolean bytes, where tile (column, row) is at position [row * numColumns + column] */
	const unsigned char *colliders;
} ShovelerViewTilemapCollidersConfiguration;

/** Adds a tilemap colliders component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddTilemapColliders(ShovelerViewEntity *entity, const ShovelerViewTilemapCollidersConfiguration *configuration);
const bool *shovelerViewEntityGetTilemapColliders(ShovelerViewEntity *entity);
/** Returns the current tilemap colliders configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetTilemapCollidersConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapCollidersConfiguration *outputConfiguration);
/** Updates a tilemap colliders component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTilemapColliders(ShovelerViewEntity *entity, const ShovelerViewTilemapCollidersConfiguration *configuration);
bool shovelerViewEntityRemoveTilemapColliders(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetTilemapCollidersComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTilemapColliders);
}

#endif
