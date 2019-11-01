#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk_layer.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewChunkLayerComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateChunkLayerType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewChunkLayerComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewChunkLayerTypeOptionKey, configuration->type);

	switch(configuration->type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewChunkLayerCanvasOptionKey, configuration->canvasEntityId);
			break;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewChunkLayerTilemapOptionKey, configuration->tilemapEntityId);
			break;
		default:
			break;
	}


	shovelerComponentActivate(component);
	return component;
}

bool shovelerViewEntityGetChunkLayerConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkLayerConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkLayerComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, shovelerViewChunkLayerTypeOptionKey);
	switch(outputConfiguration->type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewChunkLayerCanvasOptionKey);
			break;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewChunkLayerTilemapOptionKey);
			break;
		default:
			break;
	}

	return true;
}

bool shovelerViewEntityUpdateChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkLayerComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update chunk layer of entity %lld which does not have a chunk layer, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewChunkLayerTypeOptionKey, configuration->type);

	switch(configuration->type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewChunkLayerTilemapOptionKey);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewChunkLayerCanvasOptionKey, configuration->type);
			break;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewChunkLayerCanvasOptionKey);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewChunkLayerTilemapOptionKey, configuration->type);
			break;
		default:
			break;
	}

	return true;
}

bool shovelerViewEntityRemoveChunkLayer(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkLayerComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove chunk layer from entity %lld which does not have a chunk layer, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewChunkLayerComponentTypeName);
}
