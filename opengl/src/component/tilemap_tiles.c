#include "shoveler/component/tilemap_tiles.h"

#include "shoveler/component/image.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"

const char *const shovelerComponentTypeIdTilemapTiles = "tilemap_tiles";

static void *activateTilemapTilesComponent(ShovelerComponent *component);
static void deactivateTilemapTilesComponent(ShovelerComponent *component);
static bool liveUpdateTilesOption(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateTiles(ShovelerComponent *component, ShovelerTexture *texture);
static bool isComponentImageResourceEntityDefinition(ShovelerComponent *component);
static bool isComponentConfigurationOptionDefinition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilemapTilesType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[6];
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE] = shovelerComponentTypeConfigurationOptionDependency("image", shovelerComponentTypeIdImage, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS] = shovelerComponentTypeConfigurationOption("num_columns", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS] = shovelerComponentTypeConfigurationOption("num_rows", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS] = shovelerComponentTypeConfigurationOption("tileset_columns", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS] = shovelerComponentTypeConfigurationOption("tileset_rows", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS] = shovelerComponentTypeConfigurationOption("tileset_ids", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ true, liveUpdateTilesOption);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTilemapTiles, activateTilemapTilesComponent, /* update */ NULL, deactivateTilemapTilesComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerTexture *shovelerComponentGetTilemapTiles(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTilemapTiles);

	return component->data;
}

static void *activateTilemapTilesComponent(ShovelerComponent *component)
{
	bool isImageResourceEntityDefinition = isComponentImageResourceEntityDefinition(component);
	bool isConfigurationOptionDefinition = isComponentConfigurationOptionDefinition(component);

	ShovelerTexture *texture;
	if(isImageResourceEntityDefinition && !isConfigurationOptionDefinition) {
		ShovelerComponent *imageComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE);
		assert(imageComponent != NULL);
		ShovelerImage *image = shovelerComponentGetImage(imageComponent);
		assert(image != NULL);

		texture = shovelerTextureCreate2d(image, false);
		shovelerTextureUpdate(texture);
	} else if(!isImageResourceEntityDefinition && isConfigurationOptionDefinition) {
		int numColumns = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS);
		int numRows = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS);

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

static bool liveUpdateTilesOption(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	ShovelerTexture *texture = (ShovelerTexture *) component->data;
	assert(texture != NULL);

	if(!isComponentImageResourceEntityDefinition(component) && isComponentConfigurationOptionDefinition(component)) {
		updateTiles(component, texture);
	}

	return false; // don't propagate
}

static void updateTiles(ShovelerComponent *component, ShovelerTexture *texture)
{
	const unsigned char *tilesetColumns;
	const unsigned char *tilesetRows;
	const unsigned char *tilesetIds;
	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, &tilesetColumns, /* outputSize */ NULL);
	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, &tilesetRows, /* outputSize */ NULL);
	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, &tilesetIds, /* outputSize */ NULL);
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
	return shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE);
}

static bool isComponentConfigurationOptionDefinition(ShovelerComponent *component)
{
	return
		shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS) &&
		shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS) &&
		shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS) &&
		shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS) &&
		shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS);
}
