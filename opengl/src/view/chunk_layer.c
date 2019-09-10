#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk_layer.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddChunkLayer(ShovelerViewEntity *entity, const ShovelerViewChunkLayerConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewChunkLayerComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewChunkLayerComponentTypeName, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkLayerTypeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkLayerCanvasOptionKey, shovelerViewCanvasComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkLayerTilemapOptionKey, shovelerViewTilemapComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewChunkLayerComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewChunkLayerTypeOptionKey, configuration->type);

	switch(configuration->type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewChunkLayerCanvasOptionKey, configuration->type);
			break;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewChunkLayerTilemapOptionKey, configuration->type);
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

	outputConfiguration->type = shovelerComponentGetConfigurationValueUint(component, shovelerViewChunkLayerTypeOptionKey);
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
		shovelerLogWarning("Trying to update chunk layer of entity %lld which does not have a chunk layer, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewChunkLayerTypeOptionKey, configuration->type);

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
		shovelerLogWarning("Trying to remove chunk layer from entity %lld which does not have a chunk layer, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewChunkLayerComponentTypeName);
}
