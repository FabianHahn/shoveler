#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk.h"
#include "shoveler/view/chunk_layer.h"
#include "shoveler/view/position.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/log.h"

static void *activateChunkComponent(ShovelerComponent *component);
static void deactivateChunkComponent(ShovelerComponent *component);
static void updateChunkPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getChunkPosition(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewChunkComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewChunkComponentTypeName, activateChunkComponent, deactivateChunkComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkPositionOptionKey, shovelerViewPositionComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateChunkPositionDependency);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkPositionMappingXOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkPositionMappingYOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkSizeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkLayersOptionKey, shovelerViewChunkLayerComponentTypeName, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewChunkComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewChunkPositionMappingXOptionKey, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewChunkPositionMappingXOptionKey, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewChunkPositionMappingXOptionKey, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewChunkLayersOptionKey, configuration->layerEntityIds, configuration->numLayers);

	shovelerComponentActivate(component);
	return component;
}

ShovelerChunk *shovelerViewEntityGetChunk(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetChunkConfiguration(ShovelerViewEntity *entity, ShovelerViewChunkConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueUint(component, shovelerViewChunkPositionMappingXOptionKey);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueUint(component, shovelerViewChunkPositionMappingYOptionKey);
	outputConfiguration->size = shovelerComponentGetConfigurationValueVector2(component, shovelerViewChunkSizeOptionKey);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, shovelerViewChunkLayersOptionKey);
	assert(layersValue != NULL);
	outputConfiguration->layerEntityIds = layersValue->entityIdArrayValue.entityIds;
	outputConfiguration->numLayers = layersValue->entityIdArrayValue.size;

	return true;
}

bool shovelerViewEntityUpdateChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update chunk of entity %lld which does not have a chunk, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewChunkPositionMappingXOptionKey, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewChunkPositionMappingXOptionKey, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewChunkPositionMappingXOptionKey, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewChunkLayersOptionKey, configuration->layerEntityIds, configuration->numLayers);
	return true;
}

bool shovelerViewEntityRemoveChunk(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove chunk from entity %lld which does not have a chunk, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewChunkComponentTypeName);
}

static void *activateChunkComponent(ShovelerComponent *component)
{
	ShovelerVector2 chunkPosition = getChunkPosition(component);
	ShovelerVector2 size = shovelerComponentGetConfigurationValueVector2(component, shovelerViewChunkSizeOptionKey);
	ShovelerChunk *chunk = shovelerChunkCreate(chunkPosition, size);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, shovelerViewChunkLayersOptionKey);
	assert(layersValue != NULL);

	for(int i = 0; i < layersValue->entityIdArrayValue.size; i++) {
		ShovelerViewEntity *layerEntity = shovelerViewGetEntity(component->entity->view, layersValue->entityIdArrayValue.entityIds[i]);
		assert(layerEntity != NULL);
		ShovelerComponent *layerComponent = shovelerViewEntityGetChunkLayerComponent(layerEntity);
		assert(layerComponent != NULL);

		ShovelerChunkLayerType layerType = shovelerComponentGetConfigurationValueUint(layerComponent, shovelerViewChunkLayerTypeOptionKey);
		switch(layerType) {
			case SHOVELER_CHUNK_LAYER_TYPE_CANVAS: {
				long long int canvasEntityId = shovelerComponentGetConfigurationValueEntityId(layerComponent, shovelerViewChunkLayerCanvasOptionKey);
				ShovelerViewEntity *canvasEntity = shovelerViewGetEntity(component->entity->view, canvasEntityId);
				assert(canvasEntity != NULL);
				ShovelerCanvas *canvas = shovelerViewEntityGetCanvas(canvasEntity);
				assert(canvas != NULL);

				shovelerChunkAddCanvasLayer(chunk, canvas);
			} break;
			case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP: {
				long long int tilemapEntityId = shovelerComponentGetConfigurationValueEntityId(layerComponent, shovelerViewChunkLayerTilemapOptionKey);
				ShovelerViewEntity *tilemapEntity = shovelerViewGetEntity(component->entity->view, tilemapEntityId);
				assert(tilemapEntity != NULL);
				ShovelerTilemap *tilemap = shovelerViewEntityGetTilemap(tilemapEntity);
				assert(tilemap != NULL);

				shovelerChunkAddTilemapLayer(chunk, tilemap);
			} break;
			default:
				shovelerLogError("Unknown chunk layer type: %d", layerType);
				shovelerChunkFree(chunk);
				return NULL;
		}
	}

	return chunk;
}

static void deactivateChunkComponent(ShovelerComponent *component)
{
	shovelerChunkFree(component->data);
}

static void updateChunkPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerChunk *chunk = (ShovelerChunk *) component->data;
	assert(chunk != NULL);

	chunk->position = getChunkPosition(component);
}

static ShovelerVector2 getChunkPosition(ShovelerComponent *component)
{
	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewChunkPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewChunkPositionMappingXOptionKey);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewChunkPositionMappingYOptionKey);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
