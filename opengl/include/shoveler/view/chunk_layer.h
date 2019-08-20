#ifndef SHOVELER_VIEW_CHUNK_LAYER_H
#define SHOVELER_VIEW_CHUNK_LAYER_H

#include <stdbool.h> // bool

#include <shoveler/chunk.h>
#include <shoveler/component.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerChunkLayerType type;
	long long int canvasEntityId;
	long long int tilemapEntityId;
} ShovelerViewChunkLayerConfiguration;

static const char *shovelerViewChunkLayerComponentTypeName = "chunk_layer";
static const char *shovelerViewChunkLayerTypeOptionKey = "type";
static const char *shovelerViewChunkLayerCanvasOptionKey = "canvas";
static const char *shovelerViewChunkLayerTilemapOptionKey = "tilemap";

/** Adds a chunk layer component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration);
bool shovelerViewEntityGetChunkLayerConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkLayerConfiguration *outputConfiguration);
/** Updates a chunk layer component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration);
bool shovelerViewEntityRemoveChunkLayer(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetChunkLayerComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewChunkLayerComponentTypeName);
}

#endif
