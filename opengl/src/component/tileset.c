#include "shoveler/component/tileset.h"

#include <string.h> // strcmp

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/tileset.h"

static void *activateTilesetComponent(ShovelerComponent *component);
static void deactivateTilesetComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilesetType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilesetComponentTypeName, activateTilesetComponent, deactivateTilesetComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilesetImageResourceOptionKey, shovelerComponentTypeNameResource, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilesetNumColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilesetNumRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilesetPaddingOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerTileset *shovelerComponentGetTileset(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewTilesetComponentTypeName) == 0);

	return component->data;
}

static void *activateTilesetComponent(ShovelerComponent *component)
{
	ShovelerComponent *imageResourceComponent = shovelerComponentGetDependency(component, shovelerViewTilesetImageResourceOptionKey);
	assert(imageResourceComponent != NULL);
	ShovelerImage *image = shovelerComponentGetResource(imageResourceComponent);
	assert(image != NULL);

	int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilesetNumColumnsOptionKey);
	int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilesetNumRowsOptionKey);
	int padding = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilesetPaddingOptionKey);
	ShovelerTileset *tileset = shovelerTilesetCreate(image, numColumns, numRows, padding);

	return tileset;
}

static void deactivateTilesetComponent(ShovelerComponent *component)
{
	ShovelerTileset *tileset = (ShovelerTileset *) component->data;

	shovelerTilesetFree(tileset);
}
