#ifndef SHOVELER_VIEW_CHUNK_H
#define SHOVELER_VIEW_CHUNK_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/material/chunk.h>
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

static const char *shovelerViewChunkComponentTypeName = "chunk";
static const char *shovelerViewChunkPositionOptionKey = "position";
static const char *shovelerViewChunkPositionMappingXOptionKey = "position_mapping_x";
static const char *shovelerViewChunkPositionMappingYOptionKey = "position_mapping_y";
static const char *shovelerViewChunkSizeOptionKey = "size";
static const char *shovelerViewChunkLayersOptionKey = "layers";

/** Adds a chunk component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration);
ShovelerChunk *shovelerViewEntityGetChunk(ShovelerViewEntity *entity);
bool shovelerViewEntityGetChunkConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkConfiguration *outputConfiguration);
/** Updates a chunk component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration);
bool shovelerViewEntityRemoveChunk(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetChunkComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentTypeName);
}

#endif
