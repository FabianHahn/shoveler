#include "shoveler/component/tilemap_colliders.h"

#include <stdlib.h> // malloc free
#include <string.h> // strcmp

#include "shoveler/component.h"

const char *const shovelerComponentTypeIdTilemapColliders = "tilemap_colliders";

static void *activateTilemapCollidersComponent(ShovelerComponent *component);
static void deactivateTilemapCollidersComponent(ShovelerComponent *component);
static void liveUpdateCollidersOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateColliders(ShovelerComponent *component, bool *colliders);

ShovelerComponentType *shovelerComponentCreateTilemapColliders()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdTilemapColliders, activateTilemapCollidersComponent, deactivateTilemapCollidersComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapCollidersOptionKeyNumColumns, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapCollidersOptionKeyNumRows, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentTilemapCollidersOptionKeyColliders, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ false, liveUpdateCollidersOption);

	return componentType;
}

const bool *shovelerComponentGetTilemapColliders(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdTilemapColliders) == 0);

	return component->data;
}

static void *activateTilemapCollidersComponent(ShovelerComponent *component)
{
	int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapCollidersOptionKeyNumColumns);
	int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapCollidersOptionKeyNumRows);

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
	int numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapCollidersOptionKeyNumColumns);
	int numRows = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilemapCollidersOptionKeyNumRows);

	const unsigned char *collidersOption;
	shovelerComponentGetConfigurationValueBytes(component, shovelerComponentTilemapCollidersOptionKeyColliders, &collidersOption, /* outputSize */ NULL);
	assert(collidersOption != NULL);

	for(int row = 0; row < numRows; ++row) {
		int rowIndex = row * numColumns;
		for(int column = 0; column < numColumns; ++column) {
			int columnIndex = rowIndex + column;

			colliders[columnIndex] = collidersOption[columnIndex];
		}
	}
}
