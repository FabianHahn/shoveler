#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/tilemap.h"
#include "shoveler/view/tilemap_tiles.h"
#include "shoveler/view/tileset.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTilemapConfiguration configuration;
	ShovelerTilemap *tilemap;
} ComponentData;

static void assignConfiguration(ShovelerViewTilemapConfiguration *destination, const ShovelerViewTilemapConfiguration *source);
static void clearConfiguration(ShovelerViewTilemapConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add tilemap to entity %lld which already has a tilemap, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.tilesEntityId = 0;
	componentData->configuration.numTilesets = 0;
	componentData->configuration.tilesetEntityIds = NULL;
	componentData->tilemap = NULL;

	assignConfiguration(&componentData->configuration, configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, componentData->configuration.tilesEntityId, shovelerViewTilemapTilesComponentName);

	for(int i = 0; i < componentData->configuration.numTilesets; i++) {
		shovelerViewComponentAddDependency(component, componentData->configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerTilemap *shovelerViewEntityGetTilemap(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		return NULL;
	}

	ComponentData *componentData = component->data;
	return componentData->tilemap;
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

	if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.tilesEntityId, shovelerViewTilemapTilesComponentName)) {
		return false;
	}

	for(int i = 0; i < componentData->configuration.numTilesets; i++) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName)) {
			return false;
		}
	}

	assignConfiguration(&componentData->configuration, &configuration);

	shovelerViewComponentAddDependency(component, componentData->configuration.tilesEntityId, shovelerViewTilemapTilesComponentName);

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

static void assignConfiguration(ShovelerViewTilemapConfiguration *destination, const ShovelerViewTilemapConfiguration *source)
{
	clearConfiguration(destination);

	destination->tilesEntityId = source->tilesEntityId;

	destination->numTilesets = source->numTilesets;
	destination->tilesetEntityIds = malloc(destination->numTilesets * sizeof(long long int));
	memcpy(destination->tilesetEntityIds, source->tilesetEntityIds, destination->numTilesets * sizeof(long long int));
}

static void clearConfiguration(ShovelerViewTilemapConfiguration *configuration)
{
	configuration->tilesEntityId = 0;

	if(configuration->numTilesets > 0) {
		free(configuration->tilesetEntityIds);
		configuration->tilesetEntityIds = NULL;
	}
	configuration->numTilesets = 0;
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	ShovelerViewEntity *layerEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.tilesEntityId);
	assert(layerEntity != NULL);
	ShovelerTexture *tiles = shovelerViewEntityGetTilemapTiles(layerEntity);

	componentData->tilemap = shovelerTilemapCreate(tiles, NULL);

	for(int i = 0; i < componentData->configuration.numTilesets; i++) {
		ShovelerViewEntity *tilesetEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.tilesetEntityIds[i]);
		assert(tilesetEntity != NULL);
		ShovelerTileset *tileset = shovelerViewEntityGetTileset(tilesetEntity);
		assert(tileset != NULL);

		shovelerTilemapAddTileset(componentData->tilemap, tileset);
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	shovelerTilemapFree(componentData->tilemap);
	componentData->tilemap = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	clearConfiguration(&componentData->configuration);
	shovelerTilemapFree(componentData->tilemap);
	free(componentData);
}
