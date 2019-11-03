#include "shoveler/component/chunk.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/chunk_layer.h"
#include "shoveler/component/position.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/chunk.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdChunk = "chunk";

static void *activateChunkComponent(ShovelerComponent *component);
static void deactivateChunkComponent(ShovelerComponent *component);
static void updateChunkPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getChunkPosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateChunkType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[5];
	configurationOptions[SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION] = shovelerComponentTypeConfigurationOptionDependency("position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateChunkPositionDependency);
	configurationOptions[SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_X] = shovelerComponentTypeConfigurationOption("position_mapping_x", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_Y] = shovelerComponentTypeConfigurationOption("position_mapping_y", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_CHUNK_OPTION_ID_SIZE] = shovelerComponentTypeConfigurationOption("size", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_CHUNK_OPTION_ID_LAYERS] = shovelerComponentTypeConfigurationOptionDependency("layers", shovelerComponentTypeIdChunkLayer, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdChunk, activateChunkComponent, deactivateChunkComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerChunk *shovelerComponentGetChunk(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdChunk);

	return component->data;
}

static void *activateChunkComponent(ShovelerComponent *component)
{
	ShovelerVector2 chunkPosition = getChunkPosition(component);
	ShovelerVector2 size = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_SIZE);
	ShovelerChunk *chunk = shovelerChunkCreate(chunkPosition, size);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_LAYERS);
	assert(layersValue != NULL);

	for(int i = 0; i < layersValue->entityIdArrayValue.size; i++) {
		ShovelerComponent *layerComponent = shovelerComponentGetArrayDependency(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_LAYERS, i);
		assert(layerComponent != NULL);

		ShovelerChunkLayerType layerType = shovelerComponentGetConfigurationValueInt(layerComponent, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TYPE);
		switch(layerType) {
			case SHOVELER_CHUNK_LAYER_TYPE_CANVAS: {
				ShovelerComponent *canvasComponent = shovelerComponentGetDependency(layerComponent, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_CANVAS);
				assert(canvasComponent != NULL);
				ShovelerCanvas *canvas = shovelerComponentGetCanvas(canvasComponent);
				assert(canvas != NULL);

				shovelerChunkAddCanvasLayer(chunk, canvas);
			} break;
			case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP: {
				ShovelerComponent *tilemapComponent = shovelerComponentGetDependency(layerComponent, SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TILEMAP);
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

static void updateChunkPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerChunk *chunk = (ShovelerChunk *) component->data;
	assert(chunk != NULL);

	chunk->position = getChunkPosition(component);
}

static ShovelerVector2 getChunkPosition(ShovelerComponent *component)
{
	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_X);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_Y);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
