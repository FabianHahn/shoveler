#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/tilemap.h"
#include "shoveler/view/tilemap_tiles.h"
#include "shoveler/view/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

/*
 * typedef struct {
	long long int tilesEntityId;
	int numTilesets;
	const long long int *tilesetEntityIds;
} ShovelerViewTilemapConfiguration;

static const char *shovelerViewTilemapComponentTypeName = "tilemap";
static const char *shovelerViewTilemapTilesOptionKey = "tiles";
static const char *shovelerViewTilemapTilesetsOptionKey = "tilesets";
*/

static void *activateTilemapComponent(ShovelerComponent *component);
static void deactivateTilemapComponent(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTilemapComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilemapComponentTypeName, activateTilemapComponent, deactivateTilemapComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesOptionKey, shovelerViewTilemapTilesComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesetsOptionKey, shovelerViewTilesetComponentTypeName, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapTilesOptionKey, configuration->tilesEntityId);
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
		shovelerLogWarning("Trying to update tilemap of entity %lld which does not have a tilemap, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapTilesOptionKey, configuration->tilesEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewTilemapTilesetsOptionKey, configuration->tilesetEntityIds, configuration->numTilesets);
	return true;
}

bool shovelerViewEntityRemoveTilemap(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap from entity %lld which does not have a tilemap, ignoring.", entity->entityId);
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
	bool *collidingTiles = shovelerViewEntityGetTilemapCollidingTiles(tilemapEntity);

	ShovelerTilemap *tilemap = shovelerTilemapCreate(tiles, collidingTiles);

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
