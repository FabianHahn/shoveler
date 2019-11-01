#include "shoveler/component/tilemap_colliders.h"

#include <stdlib.h> // malloc free
#include <string.h> // strcmp

#include "shoveler/component.h"

static void *activateTilemapCollidersComponent(ShovelerComponent *component);
static void deactivateTilemapCollidersComponent(ShovelerComponent *component);
static void liveUpdateCollidersOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateColliders(ShovelerComponent *component, bool *colliders);

ShovelerComponentType *shovelerComponentCreateTilemapColliders()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilemapCollidersComponentTypeName, activateTilemapCollidersComponent, deactivateTilemapCollidersComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapCollidersNumColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapCollidersNumRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapCollidersCollidersOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ false, liveUpdateCollidersOption);

	return componentType;
}

const bool *shovelerComponentGetTilemapColliders(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewTilemapCollidersComponentTypeName) == 0);

	return component->data;
}

static void *activateTilemapCollidersComponent(ShovelerComponent *component)
{
	int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapCollidersNumColumnsOptionKey);
	int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapCollidersNumRowsOptionKey);

	bool *colliders = malloc(numColumns * numRows * sizeof(bool));
	updateColliders(component, colliders);

	return colliders;
}

static void deactivateTilemapCollidersComponent(ShovelerComponent *component)
{
	bool *colliders = (bool *) component->data;

	free(colliders);
}

static void liveUpdateCollidersOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	bool *colliders = (bool *) component->data;
	assert(colliders != NULL);

	updateColliders(component, colliders);
}

static void updateColliders(ShovelerComponent *component, bool *colliders)
{
	int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapCollidersNumColumnsOptionKey);
	int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapCollidersNumRowsOptionKey);

	const unsigned char *collidersOption;
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapCollidersCollidersOptionKey, &collidersOption, /* outputSize */ NULL);
	assert(collidersOption != NULL);

	for(int row = 0; row < numRows; ++row) {
		int rowIndex = row * numColumns;
		for(int column = 0; column < numColumns; ++column) {
			int columnIndex = rowIndex + column;

			colliders[columnIndex] = collidersOption[columnIndex];
		}
	}
}
