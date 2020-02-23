#include "shoveler/component/tileset.h"

#include "shoveler/component/image.h"
#include "shoveler/component.h"
#include "shoveler/tileset.h"

const char *const shovelerComponentTypeIdTileset = "tileset";

static void *activateTilesetComponent(ShovelerComponent *component);
static void deactivateTilesetComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilesetType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[4];
	configurationOptions[SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE] = shovelerComponentTypeConfigurationOptionDependency("image", shovelerComponentTypeIdImage, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS] = shovelerComponentTypeConfigurationOption("num_columns", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS] = shovelerComponentTypeConfigurationOption("num_rows", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING] = shovelerComponentTypeConfigurationOption("padding", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTileset, activateTilesetComponent, deactivateTilesetComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerTileset *shovelerComponentGetTileset(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTileset);

	return component->data;
}

static void *activateTilesetComponent(ShovelerComponent *component)
{
	ShovelerComponent *imageComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE);
	assert(imageComponent != NULL);
	ShovelerImage *image = shovelerComponentGetImage(imageComponent);
	assert(image != NULL);

	int numColumns = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS);
	int numRows = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS);
	int padding = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING);
	ShovelerTileset *tileset = shovelerTilesetCreate(image, numColumns, numRows, padding);

	return tileset;
}

static void deactivateTilesetComponent(ShovelerComponent *component)
{
	ShovelerTileset *tileset = (ShovelerTileset *) component->data;

	shovelerTilesetFree(tileset);
}
