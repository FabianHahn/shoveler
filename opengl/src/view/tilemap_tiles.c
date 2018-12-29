#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/resources.h"
#include "shoveler/view/tilemap_tiles.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTilemapTilesConfiguration configuration;
	ShovelerTexture *texture;
} ComponentData;

static void assignConfiguration(ShovelerViewTilemapTilesConfiguration *destination, const ShovelerViewTilemapTilesConfiguration *source);
static void clearConfiguration(ShovelerViewTilemapTilesConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void updateTiles(ComponentData *componentData);

bool shovelerViewEntityAddTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add tilemap tiles to entity %lld which already has tilemap tiles, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.isImageResourceEntityDefinition = false;
	componentData->configuration.imageResourceEntityId = 0;
	componentData->configuration.numColumns = 0;
	componentData->configuration.numRows = 0;
	componentData->configuration.tiles = NULL;
	componentData->texture = NULL;

	assignConfiguration(&componentData->configuration, configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapTilesComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	if(componentData->configuration.isImageResourceEntityDefinition) {
		shovelerViewComponentAddDependency(component, componentData->configuration.imageResourceEntityId, shovelerViewResourceComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerTexture *shovelerViewEntityGetTilemapTiles(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return componentData->texture;
}

const ShovelerViewTilemapTilesConfiguration *shovelerViewEntityGetTilemapTilesConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap tiles of entity %lld which does not have a tilemap tiles, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	if(componentData->configuration.isImageResourceEntityDefinition) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.imageResourceEntityId, shovelerViewResourceComponentName)) {
			return false;
		}
	}

	assignConfiguration(&componentData->configuration, configuration);

	if(componentData->configuration.isImageResourceEntityDefinition) {
		shovelerViewComponentAddDependency(component, componentData->configuration.imageResourceEntityId, shovelerViewResourceComponentName);
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateTilemapTilesData(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesTileConfiguration *tiles)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap tiles tiles of entity %lld which does not have a tilemap tiles, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;
	if(componentData->configuration.isImageResourceEntityDefinition) {
		shovelerLogWarning("Trying to update tilemap tiles tiles of entity %lld which has an image resource entity definiton, ignoring.", entity->entityId);
		return false;
	}

	memmove(componentData->configuration.tiles, tiles, componentData->configuration.numColumns * componentData->configuration.numRows * sizeof(ShovelerViewTilemapTilesTileConfiguration));

	if(component->active) {
		updateTiles(componentData);
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTilemapTiles(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap tiles from entity %lld which does not have a tilemap tiles, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapTilesComponentName);
}

static void assignConfiguration(ShovelerViewTilemapTilesConfiguration *destination, const ShovelerViewTilemapTilesConfiguration *source)
{
	clearConfiguration(destination);

	destination->isImageResourceEntityDefinition = source->isImageResourceEntityDefinition;
	destination->imageResourceEntityId = source->imageResourceEntityId;
	destination->numColumns = source->numColumns;
	destination->numRows = source->numRows;

	if(!source->isImageResourceEntityDefinition) {
		destination->tiles = malloc(destination->numColumns * destination->numRows * sizeof(ShovelerViewTilemapTilesTileConfiguration));
		memmove(destination->tiles, source->tiles, destination->numColumns * destination->numRows * sizeof(ShovelerViewTilemapTilesTileConfiguration));
	}
}

static void clearConfiguration(ShovelerViewTilemapTilesConfiguration *configuration)
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
			ShovelerViewTilemapTilesTileConfiguration tileConfiguration = componentData->configuration.tiles[row * componentData->configuration.numColumns + column];
			shovelerImageGet(tilemapImage, column, row, 0) = tileConfiguration.tilesetColumn;
			shovelerImageGet(tilemapImage, column, row, 1) = tileConfiguration.tilesetRow;
			shovelerImageGet(tilemapImage, column, row, 2) = tileConfiguration.tilesetId;
		}
	}

	shovelerTextureUpdate(componentData->texture);
}
