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

ShovelerComponent *shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdTilemap)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTilemapType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdTilemap);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES, configuration->tilesEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS, configuration->collidersEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS, configuration->tilesetEntityIds, configuration->numTilesets);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTilemap *shovelerViewEntityGetTilemap(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemap);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilemapConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemap);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->tilesEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES);
	outputConfiguration->collidersEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS);

	const ShovelerComponentConfigurationValue *layersValue = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS);
	assert(layersValue != NULL);
	outputConfiguration->tilesetEntityIds = layersValue->entityIdArrayValue.entityIds;
	outputConfiguration->numTilesets = layersValue->entityIdArrayValue.size;

	return true;
}

bool shovelerViewEntityUpdateTilemap(ShovelerViewEntity *entity, const ShovelerViewTilemapConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemap);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap of entity %lld which does not have a tilemap, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES, configuration->tilesEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS, configuration->collidersEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS, configuration->tilesetEntityIds, configuration->numTilesets);
	return true;
}

bool shovelerViewEntityRemoveTilemap(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemap);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap from entity %lld which does not have a tilemap, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdTilemap);
}
