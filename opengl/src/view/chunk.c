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
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdChunk)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateChunkType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdChunk);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_SIZE, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_LAYERS, configuration->layerEntityIds, configuration->numLayers);

	shovelerComponentActivate(component);
	return component;
}

ShovelerChunk *shovelerViewEntityGetChunk(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunk);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetChunkConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunk);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_X);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_Y);
	outputConfiguration->size = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_SIZE);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_LAYERS);
	assert(layersValue != NULL);
	outputConfiguration->layerEntityIds = layersValue->entityIdArrayValue.entityIds;
	outputConfiguration->numLayers = layersValue->entityIdArrayValue.size;

	return true;
}

bool shovelerViewEntityUpdateChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunk);
	if(component == NULL) {
		shovelerLogWarning("Trying to update chunk of entity %lld which does not have a chunk, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_SIZE, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_LAYERS, configuration->layerEntityIds, configuration->numLayers);
	return true;
}

bool shovelerViewEntityRemoveChunk(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunk);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove chunk from entity %lld which does not have a chunk, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdChunk);
}
