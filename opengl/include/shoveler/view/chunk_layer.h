#ifndef SHOVELER_VIEW_CHUNK_LAYER_H
#define SHOVELER_VIEW_CHUNK_LAYER_H

#include <stdbool.h> // bool

#include <shoveler/component/chunk_layer.h>
#include <shoveler/chunk.h>
#include <shoveler/component.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerChunkLayerType type;
	long long int canvasEntityId;
	long long int tilemapEntityId;
} ShovelerViewChunkLayerConfiguration;

/** Adds a chunk layer component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration);
bool shovelerViewEntityGetChunkLayerConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkLayerConfiguration *outputConfiguration);
/** Updates a chunk layer component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration);
bool shovelerViewEntityRemoveChunkLayer(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetChunkLayerComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunkLayer);
}

#endif
