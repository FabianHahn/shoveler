#ifndef SHOVELER_VIEW_CHUNK_H
#define SHOVELER_VIEW_CHUNK_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/component/chunk.h>
#include <shoveler/chunk.h>
#include <shoveler/tileset.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	long long int positionEntityId;
	ShovelerCoordinateMapping positionMappingX;
	ShovelerCoordinateMapping positionMappingY;
	ShovelerVector2 size;
	int numLayers;
	const long long int *layerEntityIds;
} ShovelerViewChunkConfiguration;

/** Adds a chunk component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration);
ShovelerChunk *shovelerViewEntityGetChunk(ShovelerViewEntity *entity);
/** Returns the current chunk configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetChunkConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkConfiguration *outputConfiguration);
/** Updates a chunk component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration);
bool shovelerViewEntityRemoveChunk(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetChunkComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameChunk);
}

#endif
