#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/resources.h"
#include "shoveler/view/tilemap_tiles.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTilemapTilesComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTilemapTilesType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapTilesComponentTypeName);

	if(configuration->isImageResourceEntityDefinition) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapTilesImageResourceOptionKey, configuration->imageResourceEntityId);
	} else {
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapTilesNumColumnsOptionKey, configuration->numColumns);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapTilesNumRowsOptionKey, configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapTilesTilesetColumnsOptionKey, configuration->tilesetColumns, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapTilesTilesetRowsOptionKey, configuration->tilesetRows, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapTilesTilesetIdsOptionKey, configuration->tilesetIds, configuration->numColumns * configuration->numRows);
	}

	shovelerComponentActivate(component);
	return component;
}

ShovelerTexture *shovelerViewEntityGetTilemapTiles(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilemapTilesConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapTilesConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->isImageResourceEntityDefinition = shovelerComponentHasConfigurationValue(component, shovelerViewTilemapTilesImageResourceOptionKey);

	if(outputConfiguration->isImageResourceEntityDefinition) {
		outputConfiguration->imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilemapTilesImageResourceOptionKey);
	} else {
		outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapTilesNumColumnsOptionKey);
		outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapTilesNumRowsOptionKey);
		shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapTilesTilesetColumnsOptionKey, &outputConfiguration->tilesetColumns, /* outputSize */ NULL);
		shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapTilesTilesetRowsOptionKey, &outputConfiguration->tilesetRows, /* outputSize */ NULL);
		shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapTilesTilesetIdsOptionKey, &outputConfiguration->tilesetIds, /* outputSize */ NULL);
	}

	return true;
}

bool shovelerViewEntityUpdateTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap tiles of entity %lld which does not have a tilemap tiles, ignoring.", entity->id);
		return false;
	}

	if(configuration->isImageResourceEntityDefinition) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilemapTilesImageResourceOptionKey, configuration->imageResourceEntityId);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewTilemapTilesNumColumnsOptionKey);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewTilemapTilesNumRowsOptionKey);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewTilemapTilesTilesetColumnsOptionKey);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewTilemapTilesTilesetRowsOptionKey);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewTilemapTilesTilesetIdsOptionKey);
	} else {
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerViewTilemapTilesImageResourceOptionKey);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapTilesNumColumnsOptionKey, configuration->numColumns);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapTilesNumRowsOptionKey, configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapTilesTilesetColumnsOptionKey, configuration->tilesetColumns, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapTilesTilesetRowsOptionKey, configuration->tilesetRows, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapTilesTilesetIdsOptionKey, configuration->tilesetIds, configuration->numColumns * configuration->numRows);
	}

	return true;
}

bool shovelerViewEntityRemoveTilemapTiles(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapTilesComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap tiles from entity %lld which does not have a tilemap tiles, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapTilesComponentTypeName);
}
