#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk.h"
#include "shoveler/view/chunk_layer.h"
#include "shoveler/view/position.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNameChunk)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateChunkType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNameChunk);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentChunkOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentChunkOptionKeyPositionMappingX, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentChunkOptionKeyPositionMappingX, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentChunkOptionKeySize, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerComponentChunkOptionKeyLayers, configuration->layerEntityIds, configuration->numLayers);

	shovelerComponentActivate(component);
	return component;
}

ShovelerChunk *shovelerViewEntityGetChunk(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameChunk);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetChunkConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameChunk);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentChunkOptionKeyPosition);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerComponentChunkOptionKeyPositionMappingX);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerComponentChunkOptionKeyPositionMappingY);
	outputConfiguration->size = shovelerComponentGetConfigurationValueVector2(component, shovelerComponentChunkOptionKeySize);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, shovelerComponentChunkOptionKeyLayers);
	assert(layersValue != NULL);
	outputConfiguration->layerEntityIds = layersValue->entityIdArrayValue.entityIds;
	outputConfiguration->numLayers = layersValue->entityIdArrayValue.size;

	return true;
}

bool shovelerViewEntityUpdateChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameChunk);
	if(component == NULL) {
		shovelerLogWarning("Trying to update chunk of entity %lld which does not have a chunk, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentChunkOptionKeyPositionMappingX, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentChunkOptionKeyPositionMappingX, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentChunkOptionKeyPositionMappingX, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerComponentChunkOptionKeyLayers, configuration->layerEntityIds, configuration->numLayers);
	return true;
}

bool shovelerViewEntityRemoveChunk(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameChunk);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove chunk from entity %lld which does not have a chunk, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNameChunk);
}
