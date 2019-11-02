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
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdTilemapTiles)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTilemapTilesType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdTilemapTiles);

	if(configuration->isImageResourceEntityDefinition) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTilemapTilesOptionKeyImageResource, configuration->imageResourceEntityId);
	} else {
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilemapTilesOptionKeyNumColumns, configuration->numColumns);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilemapTilesOptionKeyNumRows, configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetColumns, configuration->tilesetColumns, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetRows, configuration->tilesetRows, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetIds, configuration->tilesetIds, configuration->numColumns * configuration->numRows);
	}

	shovelerComponentActivate(component);
	return component;
}

ShovelerTexture *shovelerViewEntityGetTilemapTiles(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapTiles);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilemapTilesConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapTilesConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapTiles);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->isImageResourceEntityDefinition = shovelerComponentHasConfigurationValue(component, shovelerComponentTilemapTilesOptionKeyImageResource);

	if(outputConfiguration->isImageResourceEntityDefinition) {
		outputConfiguration->imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentTilemapTilesOptionKeyImageResource);
	} else {
		outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapTilesOptionKeyNumColumns);
		outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapTilesOptionKeyNumRows);
		shovelerComponentGetConfigurationValueBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetColumns, &outputConfiguration->tilesetColumns, /* outputSize */ NULL);
		shovelerComponentGetConfigurationValueBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetRows, &outputConfiguration->tilesetRows, /* outputSize */ NULL);
		shovelerComponentGetConfigurationValueBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetIds, &outputConfiguration->tilesetIds, /* outputSize */ NULL);
	}

	return true;
}

bool shovelerViewEntityUpdateTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapTiles);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap tiles of entity %lld which does not have a tilemap tiles, ignoring.", entity->id);
		return false;
	}

	if(configuration->isImageResourceEntityDefinition) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTilemapTilesOptionKeyImageResource, configuration->imageResourceEntityId);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerComponentTilemapTilesOptionKeyNumColumns);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerComponentTilemapTilesOptionKeyNumRows);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerComponentTilemapTilesOptionKeyTilesetColumns);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerComponentTilemapTilesOptionKeyTilesetRows);
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerComponentTilemapTilesOptionKeyTilesetIds);
	} else {
		shovelerComponentClearCanonicalConfigurationOption(component, shovelerComponentTilemapTilesOptionKeyImageResource);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilemapTilesOptionKeyNumColumns, configuration->numColumns);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilemapTilesOptionKeyNumRows, configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetColumns, configuration->tilesetColumns, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetRows, configuration->tilesetRows, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetIds, configuration->tilesetIds, configuration->numColumns * configuration->numRows);
	}

	return true;
}

bool shovelerViewEntityRemoveTilemapTiles(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapTiles);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap tiles from entity %lld which does not have a tilemap tiles, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdTilemapTiles);
}
