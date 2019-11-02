#include "shoveler/component/tileset.h"

#include <string.h> // strcmp

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/tileset.h"

static void *activateTilesetComponent(ShovelerComponent *component);
static void deactivateTilesetComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilesetType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeNameTileset, activateTilesetComponent, deactivateTilesetComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentTilesetOptionKeyImageResource, shovelerComponentTypeNameResource, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilesetOptionKeyNumColumns, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilesetOptionKeyNumRows, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilesetOptionKeyPadding, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerTileset *shovelerComponentGetTileset(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerComponentTypeNameTileset) == 0);

	return component->data;
}

static void *activateTilesetComponent(ShovelerComponent *component)
{
	ShovelerComponent *imageResourceComponent = shovelerComponentGetDependency(component, shovelerComponentTilesetOptionKeyImageResource);
	assert(imageResourceComponent != NULL);
	ShovelerImage *image = shovelerComponentGetResource(imageResourceComponent);
	assert(image != NULL);

	int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilesetOptionKeyNumColumns);
	int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilesetOptionKeyNumRows);
	int padding = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilesetOptionKeyPadding);
	ShovelerTileset *tileset = shovelerTilesetCreate(image, numColumns, numRows, padding);

	return tileset;
}

static void deactivateTilesetComponent(ShovelerComponent *component)
{
	ShovelerTileset *tileset = (ShovelerTileset *) component->data;

	shovelerTilesetFree(tileset);
}
