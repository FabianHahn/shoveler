#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk_layer.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdChunkLayer)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateChunkLayerType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdChunkLayer);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TYPE, configuration->type);

	switch(configuration->type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_CANVAS, configuration->canvasEntityId);
			break;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TILEMAP, configuration->tilemapEntityId);
			break;
		default:
			break;
	}


	shovelerComponentActivate(component);
	return component;
}

bool shovelerViewEntityGetChunkLayerConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkLayerConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunkLayer);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TYPE);
	switch(outputConfiguration->type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_CANVAS);
			break;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TILEMAP);
			break;
		default:
			break;
	}

	return true;
}

bool shovelerViewEntityUpdateChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunkLayer);
	if(component == NULL) {
		shovelerLogWarning("Trying to update chunk layer of entity %lld which does not have a chunk layer, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TYPE, configuration->type);

	switch(configuration->type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TILEMAP, /* isCanonical */ true);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_CANVAS, configuration->type);
			break;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_CANVAS, /* isCanonical */ true);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TILEMAP, configuration->type);
			break;
		default:
			break;
	}

	return true;
}

bool shovelerViewEntityRemoveChunkLayer(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdChunkLayer);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove chunk layer from entity %lld which does not have a chunk layer, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdChunkLayer);
}
