#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/resources.h"
#include "shoveler/view/tilemap_layer.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTilemapLayerConfiguration configuration;
	ShovelerTexture *texture;
} ComponentData;

static void assignConfiguration(ShovelerViewTilemapLayerConfiguration *destination, ShovelerViewTilemapLayerConfiguration *source);
static void clearConfiguration(ShovelerViewTilemapLayerConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void updateTiles(ComponentData *componentData);

bool shovelerViewEntityAddTilemapLayer(ShovelerViewEntity *entity, ShovelerViewTilemapLayerConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapLayerComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add tilemap to entity %lld which already has a tilemap, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.isImageResourceEntityDefinition = false;
	componentData->configuration.imageResourceEntityId = 0;
	componentData->configuration.numColumns = 0;
	componentData->configuration.numRows = 0;
	componentData->configuration.tiles = NULL;
	componentData->texture = NULL;

	assignConfiguration(&componentData->configuration, &configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapLayerComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	if(configuration.isImageResourceEntityDefinition) {
		shovelerViewComponentAddDependency(component, configuration.imageResourceEntityId, shovelerViewResourceComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerTexture *shovelerViewEntityGetTilemapLayer(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapLayerComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return componentData->texture;
}

const ShovelerViewTilemapLayerConfiguration *shovelerViewEntityGetTilemapLayerConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapLayerComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateTilemapLayer(ShovelerViewEntity *entity, ShovelerViewTilemapLayerConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapLayerComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap layer of entity %lld which does not have a tilemap layer, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	if(componentData->configuration.isImageResourceEntityDefinition) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.imageResourceEntityId, shovelerViewResourceComponentName)) {
			return false;
		}
	}

	assignConfiguration(&componentData->configuration, &configuration);

	if(configuration.isImageResourceEntityDefinition) {
		shovelerViewComponentAddDependency(component, configuration.imageResourceEntityId, shovelerViewResourceComponentName);
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateTilemapLayerTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapLayerTileConfiguration *tiles)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapLayerComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap layer tiles of entity %lld which does not have a tilemap layer, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;
	if(componentData->configuration.isImageResourceEntityDefinition) {
		shovelerLogWarning("Trying to update tilemap layer tiles of entity %lld which has an image resource entity definiton, ignoring.", entity->entityId);
		return false;
	}

	memmove(componentData->configuration.tiles, tiles, componentData->configuration.numColumns * componentData->configuration.numRows * sizeof(ShovelerViewTilemapLayerTileConfiguration));

	if(component->active) {
		updateTiles(componentData);
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTilemapLayer(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapLayerComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap layer from entity %lld which does not have a tilemap layer, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapLayerComponentName);
}

static void assignConfiguration(ShovelerViewTilemapLayerConfiguration *destination, ShovelerViewTilemapLayerConfiguration *source)
{
	clearConfiguration(destination);

	destination->isImageResourceEntityDefinition = source->isImageResourceEntityDefinition;
	destination->imageResourceEntityId = source->imageResourceEntityId;
	destination->numColumns = source->numColumns;
	destination->numRows = source->numRows;

	if(!source->isImageResourceEntityDefinition) {
		destination->tiles = malloc(destination->numColumns * destination->numRows * sizeof(ShovelerViewTilemapLayerTileConfiguration));
		memmove(destination->tiles, source->tiles, destination->numColumns * destination->numRows * sizeof(ShovelerViewTilemapLayerTileConfiguration));
	}
}

static void clearConfiguration(ShovelerViewTilemapLayerConfiguration *configuration)
{
	if(configuration->isImageResourceEntityDefinition) {
		free(configuration->tiles);
		configuration->tiles = NULL;
	}
	configuration->isImageResourceEntityDefinition = false;
	configuration->imageResourceEntityId = 0;
	configuration->numColumns = 0;
	configuration->numRows = 0;
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	if(componentData->configuration.isImageResourceEntityDefinition) {
		ShovelerViewEntity *imageResourceEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.imageResourceEntityId);
		assert(imageResourceEntity != NULL);
		ShovelerImage *image = shovelerViewEntityGetResource(imageResourceEntity);
		assert(image != NULL);

		componentData->texture = shovelerTextureCreate2d(image, false);
		shovelerTextureUpdate(componentData->texture);
	} else {
		ShovelerImage *tilemapImage = shovelerImageCreate(componentData->configuration.numColumns, componentData->configuration.numRows, 3);
		componentData->texture = shovelerTextureCreate2d(tilemapImage, true);
		updateTiles(componentData);
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	shovelerTextureFree(componentData->texture);
	componentData->texture = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	shovelerTextureFree(componentData->texture);

	clearConfiguration(&componentData->configuration);
	free(componentData);
}

static void updateTiles(ComponentData *componentData)
{
	assert(componentData->texture != NULL);
	ShovelerImage *tilemapImage = componentData->texture->image;

	for(int column = 0; column < componentData->configuration.numColumns; ++column) {
		for(int row = 0; row < componentData->configuration.numRows; ++row) {
			ShovelerViewTilemapLayerTileConfiguration tileConfiguration = componentData->configuration.tiles[row * componentData->configuration.numColumns + column];
			shovelerImageGet(tilemapImage, column, row, 0) = tileConfiguration.tilesetColumn;
			shovelerImageGet(tilemapImage, column, row, 1) = tileConfiguration.tilesetRow;
			shovelerImageGet(tilemapImage, column, row, 2) = tileConfiguration.tilesetId;
		}
	}

	shovelerTextureUpdate(componentData->texture);
}
