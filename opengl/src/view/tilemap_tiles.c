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
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE, configuration->imageEntityId);
	} else {
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS, configuration->numColumns);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS, configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, configuration->tilesetColumns, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, configuration->tilesetRows, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, configuration->tilesetIds, configuration->numColumns * configuration->numRows);
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

	outputConfiguration->isImageResourceEntityDefinition = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE);

	if(outputConfiguration->isImageResourceEntityDefinition) {
		outputConfiguration->imageEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE);
	} else {
		outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS);
		outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS);
		shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, &outputConfiguration->tilesetColumns, /* outputSize */ NULL);
		shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, &outputConfiguration->tilesetRows, /* outputSize */ NULL);
		shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, &outputConfiguration->tilesetIds, /* outputSize */ NULL);
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
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE, configuration->imageEntityId);
		shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS, /* isCanonical */ true);
		shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS, /* isCanonical */ true);
		shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, /* isCanonical */ true);
		shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, /* isCanonical */ true);
		shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, /* isCanonical */ true);
	} else {
		shovelerComponentClearConfigurationOption(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE, /* isCanonical */ true);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS, configuration->numColumns);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS, configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, configuration->tilesetColumns, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, configuration->tilesetRows, configuration->numColumns * configuration->numRows);
		shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, configuration->tilesetIds, configuration->numColumns * configuration->numRows);
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
