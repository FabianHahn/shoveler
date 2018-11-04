#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/tilemap.h"
#include "shoveler/view/tilemap_layer.h"
#include "shoveler/view/tileset.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTilemapConfiguration configuration;
	int numLayers;
	ShovelerTexture **layers;
	int numTilesets;
	ShovelerTileset **tilesets;
} ComponentData;

static void assignConfiguration(ShovelerViewTilemapConfiguration *destination, ShovelerViewTilemapConfiguration *source);
static void clearConfiguration(ShovelerViewTilemapConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add tilemap to entity %lld which already has a tilemap, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.numLayers = 0;
	componentData->configuration.layerEntityIds = NULL;
	componentData->configuration.numTilesets = 0;
	componentData->configuration.tilesetEntityIds = NULL;
	componentData->numLayers = 0;
	componentData->layers = NULL;
	componentData->numTilesets = 0;
	componentData->tilesets = NULL;

	assignConfiguration(&componentData->configuration, &configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	for(int i = 0; i < configuration.numLayers; i++) {
		shovelerViewComponentAddDependency(component, configuration.layerEntityIds[i], shovelerViewTilemapLayerComponentName);
	}
	for(int i = 0; i < configuration.numTilesets; i++) {
		shovelerViewComponentAddDependency(component, configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

bool shovelerViewEntityGetTilemapLayers(ShovelerViewEntity *entity, int *numLayersPointer, ShovelerTexture ***layersPointer)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	*numLayersPointer = componentData->numLayers;
	*layersPointer = componentData->layers;
	return true;
}

bool shovelerViewEntityGetTilemapTilesets(ShovelerViewEntity *entity, int *numTilesetsPointer, ShovelerTileset ***tilesetsPointer)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	*numTilesetsPointer = componentData->numTilesets;
	*tilesetsPointer = componentData->tilesets;
	return true;
}

const ShovelerViewTilemapConfiguration *shovelerViewEntityGetTilemapConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateTilemap(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap of entity %lld which does not have a tilemap, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.layerEntityIds[i], shovelerViewTilemapLayerComponentName)) {
			return false;
		}
	}
	for(int i = 0; i < componentData->configuration.numTilesets; i++) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName)) {
			return false;
		}
	}

	assignConfiguration(&componentData->configuration, &configuration);

	for(int i = 0; i < configuration.numLayers; i++) {
		shovelerViewComponentAddDependency(component, configuration.layerEntityIds[i], shovelerViewTilemapLayerComponentName);
	}
	for(int i = 0; i < configuration.numTilesets; i++) {
		shovelerViewComponentAddDependency(component, configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName);
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTilemap(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap from entity %lld which does not have a tilemap, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapComponentName);
}

static void assignConfiguration(ShovelerViewTilemapConfiguration *destination, ShovelerViewTilemapConfiguration *source)
{
	clearConfiguration(destination);

	destination->numLayers = source->numLayers;
	destination->layerEntityIds = malloc(destination->numLayers * sizeof(long long int));
	memcpy(destination->layerEntityIds, source->layerEntityIds, destination->numLayers * sizeof(long long int));

	destination->numTilesets = source->numTilesets;
	destination->tilesetEntityIds = malloc(destination->numTilesets * sizeof(long long int));
	memcpy(destination->tilesetEntityIds, source->tilesetEntityIds, destination->numTilesets * sizeof(long long int));
}

static void clearConfiguration(ShovelerViewTilemapConfiguration *configuration)
{
	if(configuration->numLayers > 0) {
		free(configuration->layerEntityIds);
		configuration->layerEntityIds = NULL;
	}
	configuration->numLayers = 0;

	if(configuration->numTilesets > 0) {
		free(configuration->tilesetEntityIds);
		configuration->tilesetEntityIds = NULL;
	}
	configuration->numTilesets = 0;
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	componentData->numLayers = componentData->configuration.numLayers;
	componentData->layers = malloc(componentData->numLayers * sizeof(ShovelerTexture *));

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		ShovelerViewEntity *layerEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.layerEntityIds[i]);
		assert(layerEntity != NULL);
		componentData->layers[i] = shovelerViewEntityGetTilemapLayer(layerEntity);
		assert(componentData->layers[i] != NULL);
	}

	componentData->numTilesets = componentData->configuration.numTilesets;
	componentData->tilesets = malloc(componentData->numTilesets * sizeof(ShovelerTileset *));

	for(int i = 0; i < componentData->configuration.numTilesets; i++) {
		ShovelerViewEntity *tilesetEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.tilesetEntityIds[i]);
		assert(tilesetEntity != NULL);
		componentData->tilesets[i] = shovelerViewEntityGetTileset(tilesetEntity);
		assert(componentData->tilesets[i] != NULL);
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	free(componentData->layers);
	componentData->numLayers = 0;
	componentData->layers = NULL;

	free(componentData->tilesets);
	componentData->numTilesets = 0;
	componentData->tilesets = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	clearConfiguration(&componentData->configuration);
	free(componentData->layers);
	free(componentData->tilesets);
	free(componentData);
}
