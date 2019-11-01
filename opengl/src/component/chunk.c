#include "shoveler/component/chunk.h"

#include <string.h> // strcmp

#include "shoveler/component/canvas.h"
#include "shoveler/component/chunk_layer.h"
#include "shoveler/component/position.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/chunk.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

static void *activateChunkComponent(ShovelerComponent *component);
static void deactivateChunkComponent(ShovelerComponent *component);
static void updateChunkPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getChunkPosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateChunkType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewChunkComponentTypeName, activateChunkComponent, deactivateChunkComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkPositionOptionKey, shovelerComponentTypeNamePosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateChunkPositionDependency);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkPositionMappingXOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkPositionMappingYOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkSizeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkLayersOptionKey, shovelerViewChunkLayerComponentTypeName, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}

ShovelerChunk *shovelerComponentGetChunk(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewChunkComponentTypeName) == 0);

	return component->data;
}

static void *activateChunkComponent(ShovelerComponent *component)
{
	ShovelerVector2 chunkPosition = getChunkPosition(component);
	ShovelerVector2 size = shovelerComponentGetConfigurationValueVector2(component, shovelerViewChunkSizeOptionKey);
	ShovelerChunk *chunk = shovelerChunkCreate(chunkPosition, size);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, shovelerViewChunkLayersOptionKey);
	assert(layersValue != NULL);

	for(int i = 0; i < layersValue->entityIdArrayValue.size; i++) {
		ShovelerComponent *layerComponent = shovelerComponentGetArrayDependency(component, shovelerViewChunkLayersOptionKey, i);
		assert(layerComponent != NULL);

		ShovelerChunkLayerType layerType = shovelerComponentGetConfigurationValueInt(layerComponent, shovelerViewChunkLayerTypeOptionKey);
		switch(layerType) {
			case SHOVELER_CHUNK_LAYER_TYPE_CANVAS: {
				ShovelerComponent *canvasComponent = shovelerComponentGetDependency(layerComponent, shovelerViewChunkLayerCanvasOptionKey);
				assert(canvasComponent != NULL);
				ShovelerCanvas *canvas = shovelerComponentGetCanvas(canvasComponent);
				assert(canvas != NULL);

				shovelerChunkAddCanvasLayer(chunk, canvas);
			} break;
			case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP: {
				ShovelerComponent *tilemapComponent = shovelerComponentGetDependency(layerComponent, shovelerViewChunkLayerTilemapOptionKey);
				assert(tilemapComponent != NULL);
				ShovelerTilemap *tilemap = shovelerComponentGetTilemap(tilemapComponent);
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
	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerViewChunkPositionOptionKey);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerViewChunkPositionMappingXOptionKey);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerViewChunkPositionMappingYOptionKey);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
