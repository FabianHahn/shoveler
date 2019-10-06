#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/resources.h"
#include "shoveler/view/tilemap_tiles.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

static void *activateTilemapTilesComponent(ShovelerComponent *component);
static void deactivateTilemapTilesComponent(ShovelerComponent *component);
static void liveUpdateTilesOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateTiles(ShovelerComponent *component, ShovelerTexture *texture);
static bool isComponentImageResourceEntityDefinition(ShovelerComponent *component);
static bool isComponentConfigurationOptionDefinition(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddTilemapTiles(ShovelerViewEntity *entity, const ShovelerViewTilemapTilesConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTilemapTilesComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilemapTilesComponentTypeName, activateTilemapTilesComponent, deactivateTilemapTilesComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesImageResourceOptionKey, shovelerViewResourceComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesNumColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesNumRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesTilesetColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesTilesetRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesTilesetIdsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
		shovelerViewAddComponentType(entity->view, componentType);
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
		shovelerLogWarning("Trying to update tilemap tiles of entity %lld which does not have a tilemap tiles, ignoring.", entity->entityId);
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
		shovelerLogWarning("Trying to remove tilemap tiles from entity %lld which does not have a tilemap tiles, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapTilesComponentTypeName);
}

static void *activateTilemapTilesComponent(ShovelerComponent *component)
{
	bool isImageResourceEntityDefinition = isComponentImageResourceEntityDefinition(component);
	bool isConfigurationOptionDefinition = isComponentConfigurationOptionDefinition(component);

	ShovelerTexture *texture;
	if(isImageResourceEntityDefinition && !isConfigurationOptionDefinition) {
		long long int imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilemapTilesImageResourceOptionKey);
		ShovelerViewEntity *imageResourceEntity = shovelerViewGetEntity(component->entity->view, imageResourceEntityId);
		assert(imageResourceEntity != NULL);
		ShovelerImage *image = shovelerViewEntityGetResource(imageResourceEntity);
		assert(image != NULL);

		texture = shovelerTextureCreate2d(image, false);
		shovelerTextureUpdate(texture);
	} else if(!isImageResourceEntityDefinition && isConfigurationOptionDefinition) {
		int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapTilesNumColumnsOptionKey);
		int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapTilesNumRowsOptionKey);

		ShovelerImage *tilemapImage = shovelerImageCreate(numColumns, numRows, /* channels */ 3);
		texture = shovelerTextureCreate2d(tilemapImage, true);
		updateTiles(component, texture);
	} else {
		shovelerLogWarning("Failed to activate tilemap tiles of entity %lld because it doesn't provide either an image resource entity definition or a configuration option definition", component->entity->entityId);
		return NULL;
	}

	return texture;
}

static void deactivateTilemapTilesComponent(ShovelerComponent *component)
{
	ShovelerTexture *texture = (ShovelerTexture *) component->data;

	shovelerTextureFree(texture);
}

static void liveUpdateTilesOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	ShovelerTexture *texture = (ShovelerTexture *) component->data;
	assert(texture != NULL);

	if(!isComponentImageResourceEntityDefinition(component) && isComponentConfigurationOptionDefinition(component)) {
		updateTiles(component, texture);
	}
}

static void updateTiles(ShovelerComponent *component, ShovelerTexture *texture)
{
	const unsigned char *tilesetColumns;
	const unsigned char *tilesetRows;
	const unsigned char *tilesetIds;
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapTilesTilesetColumnsOptionKey, &tilesetColumns, /* outputSize */ NULL);
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapTilesTilesetRowsOptionKey, &tilesetRows, /* outputSize */ NULL);
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapTilesTilesetIdsOptionKey, &tilesetIds, /* outputSize */ NULL);
	assert(tilesetColumns != NULL);
	assert(tilesetRows != NULL);
	assert(tilesetIds != NULL);

	ShovelerImage *tilemapImage = texture->image;
	int numColumns = tilemapImage->width;
	int numRows = tilemapImage->height;

	for(int row = 0; row < numRows; ++row) {
		int rowIndex = row * numColumns;
		for(int column = 0; column < numColumns; ++column) {
			int columnIndex = rowIndex + column;

			shovelerImageGet(tilemapImage, column, row, 0) = tilesetColumns[columnIndex];
			shovelerImageGet(tilemapImage, column, row, 1) = tilesetRows[columnIndex];
			shovelerImageGet(tilemapImage, column, row, 2) = tilesetIds[columnIndex];
		}
	}

	shovelerTextureUpdate(texture);
}

static bool isComponentImageResourceEntityDefinition(ShovelerComponent *component)
{
	return shovelerComponentHasConfigurationValue(component, shovelerViewTilemapTilesImageResourceOptionKey);
}

static bool isComponentConfigurationOptionDefinition(ShovelerComponent *component)
{
	return
		shovelerComponentHasConfigurationValue(component, shovelerViewTilemapTilesNumColumnsOptionKey) &&
		shovelerComponentHasConfigurationValue(component, shovelerViewTilemapTilesNumRowsOptionKey) &&
		shovelerComponentHasConfigurationValue(component, shovelerViewTilemapTilesTilesetColumnsOptionKey) &&
		shovelerComponentHasConfigurationValue(component, shovelerViewTilemapTilesTilesetRowsOptionKey) &&
		shovelerComponentHasConfigurationValue(component, shovelerViewTilemapTilesTilesetIdsOptionKey);
}
