#include "shoveler/component/tilemap_tiles.h"

#include <string.h> // strcmp

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"

const char *const shovelerComponentTypeIdTilemapTiles = "tilemap_tiles";

static void *activateTilemapTilesComponent(ShovelerComponent *component);
static void deactivateTilemapTilesComponent(ShovelerComponent *component);
static void liveUpdateTilesOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateTiles(ShovelerComponent *component, ShovelerTexture *texture);
static bool isComponentImageResourceEntityDefinition(ShovelerComponent *component);
static bool isComponentConfigurationOptionDefinition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilemapTilesType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdTilemapTiles, activateTilemapTilesComponent, deactivateTilemapTilesComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentTilemapTilesOptionKeyImageResource, shovelerComponentTypeIdResource, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapTilesOptionKeyNumColumns, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapTilesOptionKeyNumRows, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapTilesOptionKeyTilesetColumns, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapTilesOptionKeyTilesetRows, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapTilesOptionKeyTilesetIds, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);

	return componentType;
}

ShovelerTexture *shovelerComponentGetTilemapTiles(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdTilemapTiles) == 0);

	return component->data;
}

static void *activateTilemapTilesComponent(ShovelerComponent *component)
{
	bool isImageResourceEntityDefinition = isComponentImageResourceEntityDefinition(component);
	bool isConfigurationOptionDefinition = isComponentConfigurationOptionDefinition(component);

	ShovelerTexture *texture;
	if(isImageResourceEntityDefinition && !isConfigurationOptionDefinition) {
		ShovelerComponent *imageResourceComponent = shovelerComponentGetDependency(component, shovelerComponentTilemapTilesOptionKeyImageResource);
		assert(imageResourceComponent != NULL);
		ShovelerImage *image = shovelerComponentGetResource(imageResourceComponent);
		assert(image != NULL);

		texture = shovelerTextureCreate2d(image, false);
		shovelerTextureUpdate(texture);
	} else if(!isImageResourceEntityDefinition && isConfigurationOptionDefinition) {
		int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapTilesOptionKeyNumColumns);
		int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapTilesOptionKeyNumRows);

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
	shovelerComponentGetConfigurationValueBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetColumns, &tilesetColumns, /* outputSize */ NULL);
	shovelerComponentGetConfigurationValueBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetRows, &tilesetRows, /* outputSize */ NULL);
	shovelerComponentGetConfigurationValueBytes(component, shovelerComponentTilemapTilesOptionKeyTilesetIds, &tilesetIds, /* outputSize */ NULL);
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
	return shovelerComponentHasConfigurationValue(component, shovelerComponentTilemapTilesOptionKeyImageResource);
}

static bool isComponentConfigurationOptionDefinition(ShovelerComponent *component)
{
	return
		shovelerComponentHasConfigurationValue(component, shovelerComponentTilemapTilesOptionKeyNumColumns) &&
		shovelerComponentHasConfigurationValue(component, shovelerComponentTilemapTilesOptionKeyNumRows) &&
		shovelerComponentHasConfigurationValue(component, shovelerComponentTilemapTilesOptionKeyTilesetColumns) &&
		shovelerComponentHasConfigurationValue(component, shovelerComponentTilemapTilesOptionKeyTilesetRows) &&
		shovelerComponentHasConfigurationValue(component, shovelerComponentTilemapTilesOptionKeyTilesetIds);
}
