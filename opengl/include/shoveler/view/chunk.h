#ifndef SHOVELER_VIEW_CHUNK_H
#define SHOVELER_VIEW_CHUNK_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/material/chunk.h>
#include <shoveler/chunk.h>
#include <shoveler/tileset.h>
#include <shoveler/view.h>

typedef enum {
	SHOVELER_VIEW_CHUNK_COORDINATE_MAPPING_POSITIVE_X,
	SHOVELER_VIEW_CHUNK_COORDINATE_MAPPING_NEGATIVE_X,
	SHOVELER_VIEW_CHUNK_COORDINATE_MAPPING_POSITIVE_Y,
	SHOVELER_VIEW_CHUNK_COORDINATE_MAPPING_NEGATIVE_Y,
	SHOVELER_VIEW_CHUNK_COORDINATE_MAPPING_POSITIVE_Z,
	SHOVELER_VIEW_CHUNK_COORDINATE_MAPPING_NEGATIVE_Z,
} ShovelerViewChunkCoordinateMappingConfiguration;

typedef struct {
	ShovelerChunkLayerType type;
	long long int valueEntityId;
} ShovelerViewChunkLayerConfiguration;

typedef struct {
	ShovelerViewChunkCoordinateMappingConfiguration positionMappingX;
	ShovelerViewChunkCoordinateMappingConfiguration positionMappingY;
	ShovelerVector2 size;
	int numLayers;
	ShovelerViewChunkLayerConfiguration *layers;
} ShovelerViewChunkConfiguration;

static const char *shovelerViewChunkComponentName = "chunk";

/** Adds a chunk component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration);
ShovelerChunk *shovelerViewEntityGetChunk(ShovelerViewEntity *entity);
const ShovelerViewChunkConfiguration *shovelerViewEntityGetChunkConfiguration(ShovelerViewEntity *entity);
/** Updates a chunk component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateChunk(ShovelerViewEntity *entity, ShovelerViewChunkConfiguration configuration);
bool shovelerViewEntityRemoveChunk(ShovelerViewEntity *entity);

#endif
