#include "shoveler/component/tilemap_tiles.h"

#include <string.h> // strcmp

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"

static void *activateTilemapTilesComponent(ShovelerComponent *component);
static void deactivateTilemapTilesComponent(ShovelerComponent *component);
static void liveUpdateTilesOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateTiles(ShovelerComponent *component, ShovelerTexture *texture);
static bool isComponentImageResourceEntityDefinition(ShovelerComponent *component);
static bool isComponentConfigurationOptionDefinition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilemapTilesType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilemapTilesComponentTypeName, activateTilemapTilesComponent, deactivateTilemapTilesComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesImageResourceOptionKey, shovelerComponentTypeNameResource, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesNumColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesNumRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesTilesetColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesTilesetRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapTilesTilesetIdsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);

	return componentType;
}

ShovelerTexture *shovelerComponentGetTilemapTiles(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewTilemapTilesComponentTypeName) == 0);

	return component->data;
}

static void *activateTilemapTilesComponent(ShovelerComponent *component)
{
	bool isImageResourceEntityDefinition = isComponentImageResourceEntityDefinition(component);
	bool isConfigurationOptionDefinition = isComponentConfigurationOptionDefinition(component);

	ShovelerTexture *texture;
	if(isImageResourceEntityDefinition && !isConfigurationOptionDefinition) {
		ShovelerComponent *imageResourceComponent = shovelerComponentGetDependency(component, shovelerViewTilemapTilesImageResourceOptionKey);
		assert(imageResourceComponent != NULL);
		ShovelerImage *image = shovelerComponentGetResource(imageResourceComponent);
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
		shovelerLogWarning("Failed to activate tilemap tiles of entity %lld because it doesn't provide either an image resource entity definition or a configuration option definition", component->entityId);
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
