#include "shoveler/component/tileset.h"

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/tileset.h"

const char *const shovelerComponentTypeIdTileset = "tileset";

static void *activateTilesetComponent(ShovelerComponent *component);
static void deactivateTilesetComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilesetType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[4];
	configurationOptions[SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE_RESOURCE] = shovelerComponentTypeConfigurationOptionDependency("image_resource", shovelerComponentTypeIdResource, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
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
	ShovelerComponent *imageResourceComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE_RESOURCE);
	assert(imageResourceComponent != NULL);
	ShovelerImage *image = shovelerComponentGetResource(imageResourceComponent);
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
