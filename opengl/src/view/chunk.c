#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/collider/chunk.h"
#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk.h"
#include "shoveler/view/colliders.h"
#include "shoveler/view/position.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/colliders.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewChunkConfiguration configuration;
	ShovelerChunk *chunk;
	ShovelerCollider2 *collider;
	ShovelerViewComponentCallback *positionCallback;
} ComponentData;

static void assignConfiguration(ShovelerViewChunkConfiguration *destination, const ShovelerViewChunkConfiguration *source);
static void clearConfiguration(ShovelerViewChunkConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *componentDataPointer);
static void updatePosition(ComponentData *componentData, const ShovelerViewPosition *position);
static const char *layerTypeToComponentName(ShovelerChunkLayerType type);
static double mapCoordinate(const ShovelerViewPosition *position, ShovelerCoordinateMapping mapping);

bool shovelerViewEntityAddChunk(ShovelerViewEntity *entity, const ShovelerViewChunkConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add chunk to entity %lld which already has a chunk, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.positionMappingX = SHOVELER_COORDINATE_MAPPING_POSITIVE_X;
	componentData->configuration.positionMappingY = SHOVELER_COORDINATE_MAPPING_POSITIVE_Y;
	componentData->configuration.size = shovelerVector2(0.0f, 0.0f);
	componentData->configuration.collider = false;
	componentData->configuration.numLayers = 0;
	componentData->configuration.layers = NULL;
	componentData->chunk = NULL;
	componentData->collider = NULL;
	componentData->positionCallback = NULL;

	assignConfiguration(&componentData->configuration, configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewChunkComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewPositionComponentName);

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		const ShovelerViewChunkLayerConfiguration *layerConfiguration = &componentData->configuration.layers[i];
		shovelerViewComponentAddDependency(component, layerConfiguration->valueEntityId, layerTypeToComponentName(layerConfiguration->type));
	}

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerChunk *shovelerViewEntityGetChunk(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentName);
	if(component == NULL) {
		return NULL;
	}

	ComponentData *componentData = component->data;
	return componentData->chunk;
}

const ShovelerViewChunkConfiguration *shovelerViewEntityGetChunkConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateChunk(ShovelerViewEntity *entity, ShovelerViewChunkConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update chunk of entity %lld which does not have a chunk, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		const ShovelerViewChunkLayerConfiguration *layerConfiguration = &componentData->configuration.layers[i];
		if(!shovelerViewComponentRemoveDependency(component, layerConfiguration->valueEntityId, layerTypeToComponentName(layerConfiguration->type))) {
			return false;
		}
	}

	assignConfiguration(&componentData->configuration, &configuration);

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		const ShovelerViewChunkLayerConfiguration *layerConfiguration = &componentData->configuration.layers[i];
		shovelerViewComponentAddDependency(component, layerConfiguration->valueEntityId, layerTypeToComponentName(layerConfiguration->type));
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveChunk(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewChunkComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove chunk from entity %lld which does not have a chunk, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewChunkComponentName);
}

static void assignConfiguration(ShovelerViewChunkConfiguration *destination, const ShovelerViewChunkConfiguration *source)
{
	clearConfiguration(destination);

	destination->positionMappingX = source->positionMappingX;
	destination->positionMappingY = source->positionMappingY;
	destination->size = source->size;
	destination->collider = source->collider;
	destination->numLayers = source->numLayers;
	destination->layers = malloc(destination->numLayers * sizeof(ShovelerViewChunkLayerConfiguration));
	memcpy(destination->layers, source->layers, destination->numLayers * sizeof(ShovelerViewChunkLayerConfiguration));
}

static void clearConfiguration(ShovelerViewChunkConfiguration *configuration)
{
	configuration->positionMappingX = SHOVELER_COORDINATE_MAPPING_POSITIVE_X;
	configuration->positionMappingY = SHOVELER_COORDINATE_MAPPING_POSITIVE_Y;
	configuration->size = shovelerVector2(0.0f, 0.0f);
	configuration->collider = false;

	if(configuration->numLayers > 0) {
		free(configuration->layers);
		configuration->layers = NULL;
	}
	configuration->numLayers = 0;
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	assert(shovelerViewHasColliders(component->entity->view));
	ComponentData *componentData = componentDataPointer;

	componentData->chunk = shovelerChunkCreate(shovelerVector2(0.0f, 0.0f), componentData->configuration.size);

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		const ShovelerViewChunkLayerConfiguration *layerConfiguration = &componentData->configuration.layers[i];
		ShovelerViewEntity *layerEntity = shovelerViewGetEntity(component->entity->view, layerConfiguration->valueEntityId);
		assert(layerEntity != NULL);

		switch(layerConfiguration->type) {
			case SHOVELER_CHUNK_LAYER_TYPE_CANVAS: {
				ShovelerCanvas *canvas = shovelerViewEntityGetCanvas(layerEntity);
				assert(canvas != NULL);

				shovelerChunkAddCanvasLayer(componentData->chunk, canvas);
			} break;
			case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP: {
				ShovelerTilemap *tilemap = shovelerViewEntityGetTilemap(layerEntity);
				assert(tilemap != NULL);

				shovelerChunkAddTilemapLayer(componentData->chunk, tilemap);
			} break;
			default:
				shovelerLogError("Unknown chunk layer type: %d", layerConfiguration->type);
				shovelerChunkFree(componentData->chunk);
				componentData->chunk = NULL;
				return false;
		}
	}

	updatePosition(componentData, shovelerViewEntityGetPosition(component->entity));

	if(componentData->configuration.collider) {
		ShovelerColliders *colliders = shovelerViewGetColliders(component->entity->view);

		componentData->collider = shovelerColliderChunkCreate(componentData->chunk);
		shovelerCollidersAddCollider2(colliders, componentData->collider);
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	assert(shovelerViewHasColliders(component->entity->view));
	ComponentData *componentData = componentDataPointer;

	if(componentData->configuration.collider) {
		ShovelerColliders *colliders = shovelerViewGetColliders(component->entity->view);
		shovelerCollidersRemoveCollider2(colliders, componentData->collider);

		shovelerCollider2Free(componentData->collider);
		componentData->collider = NULL;
	}

	shovelerChunkFree(componentData->chunk);
	componentData->chunk = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	clearConfiguration(&componentData->configuration);

	shovelerChunkFree(componentData->chunk);
	shovelerCollider2Free(componentData->collider);

	free(componentData);
}

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	updatePosition(componentData, shovelerViewEntityGetPosition(positionComponent->entity));
}

static void updatePosition(ComponentData *componentData, const ShovelerViewPosition *position)
{
	ShovelerVector3 positionCoordinates = shovelerVector3((float) position->x, (float) position->y, (float) position->z);

	componentData->chunk->position.values[0] = shovelerCoordinateMap(positionCoordinates, componentData->configuration.positionMappingX);
	componentData->chunk->position.values[1] = shovelerCoordinateMap(positionCoordinates, componentData->configuration.positionMappingY);
}

static const char *layerTypeToComponentName(ShovelerChunkLayerType type)
{
	switch(type) {
		case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
			return shovelerViewCanvasComponentName;
		case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
			return shovelerViewTilemapComponentName;
		default:
			shovelerLogError("Unknown chunk layer type: %d", type);
			return NULL;
	}
}
