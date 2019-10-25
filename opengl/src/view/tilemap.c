#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/tilemap.h"
#include "shoveler/view/tilemap_colliders.h"
#include "shoveler/view/tilemap_tiles.h"
#include "shoveler/view/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

static void *activateTilemapComponent(ShovelerComponent *component);
static void deactivateTilemapComponent(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTilemapComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilemapComponentTypeName, activateTilemapComponent, deactivateTilemapComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesOptionKey, shovelerViewTilemapTilesComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapCollidersOptionKey, shovelerViewTilemapCollidersComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesetsOptionKey, shovelerViewTilesetComponentTypeName, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapTilesOptionKey, configuration->tilesEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapCollidersOptionKey, configuration->collidersEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewTilemapTilesetsOptionKey, configuration->tilesetEntityIds, configuration->numTilesets);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTilemap *shovelerViewEntityGetTilemap(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilemapConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->tilesEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilemapTilesOptionKey);
	outputConfiguration->collidersEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilemapCollidersOptionKey);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, shovelerViewTilemapTilesetsOptionKey);
	assert(layersValue != NULL);
	outputConfiguration->tilesetEntityIds = layersValue->entityIdArrayValue.entityIds;
	outputConfiguration->numTilesets = layersValue->entityIdArrayValue.size;

	return true;
}

bool shovelerViewEntityUpdateTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap of entity %lld which does not have a tilemap, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapTilesOptionKey, configuration->tilesEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapCollidersOptionKey, configuration->collidersEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewTilemapTilesetsOptionKey, configuration->tilesetEntityIds, configuration->numTilesets);
	return true;
}

bool shovelerViewEntityRemoveTilemap(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap from entity %lld which does not have a tilemap, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapComponentTypeName);
}

static void *activateTilemapComponent(ShovelerComponent *component)
{
	long long int tilesEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilemapTilesOptionKey);
	ShovelerViewEntity *tilemapEntity = shovelerViewGetEntity(component->entity->view, tilesEntityId);
	assert(tilemapEntity != NULL);
	ShovelerTexture *tiles = shovelerViewEntityGetTilemapTiles(tilemapEntity);
	assert(tiles != NULL);
	int numTilesColumns = tiles->width;
	int numTilesRows = tiles->height;

	long long int collidersEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilemapCollidersOptionKey);
	ShovelerViewEntity *collidersEntity = shovelerViewGetEntity(component->entity->view, collidersEntityId);
	assert(collidersEntity != NULL);
	ShovelerComponent *collidersComponent = shovelerViewEntityGetTilemapCollidersComponent(collidersEntity);
	assert(collidersComponent != NULL);
	const bool *colliders = shovelerViewEntityGetTilemapColliders(collidersEntity);
	assert(colliders != NULL);
	int numCollidersColumns = shovelerComponentGetConfigurationValueInt(collidersComponent, shovelerViewTilemapCollidersNumColumnsOptionKey);
	int numCollidersRows = shovelerComponentGetConfigurationValueInt(collidersComponent, shovelerViewTilemapCollidersNumRowsOptionKey);

	if(numTilesColumns != numCollidersColumns || numTilesRows != numCollidersRows) {
		shovelerLogWarning("Failed to activate tilemap %lld because dependency tiles dimensions don't match dependency colliders dimensions.", component->entity->entityId);
		return NULL;
	}

	ShovelerTilemap *tilemap = shovelerTilemapCreate(tiles, colliders);

	const ShovelerComponentConfigurationValue *tilesetsValue = shovelerComponentGetConfigurationValue(component, shovelerViewTilemapTilesetsOptionKey);
	assert(tilesetsValue != NULL);

	for(int i = 0; i < tilesetsValue->entityIdArrayValue.size; i++) {
		ShovelerViewEntity *tilesetEntity = shovelerViewGetEntity(component->entity->view, tilesetsValue->entityIdArrayValue.entityIds[i]);
		assert(tilesetEntity != NULL);
		ShovelerTileset *tileset = shovelerViewEntityGetTileset(tilesetEntity);
		assert(tileset != NULL);

		shovelerTilemapAddTileset(tilemap, tileset);
	}

	return tilemap;
}

static void deactivateTilemapComponent(ShovelerComponent *component)
{
	ShovelerTilemap *tilemap = (ShovelerTilemap *) component->data;

	shovelerTilemapFree(tilemap);
}
