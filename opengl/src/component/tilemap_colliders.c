#include "shoveler/component/tilemap_colliders.h"

#include <stdlib.h> // malloc free

#include "shoveler/component.h"

const char *const shovelerComponentTypeIdTilemapColliders = "tilemap_colliders";

static void *activateTilemapCollidersComponent(ShovelerComponent *component);
static void deactivateTilemapCollidersComponent(ShovelerComponent *component);
static void liveUpdateCollidersOption(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateColliders(ShovelerComponent *component, bool *colliders);

ShovelerComponentType *shovelerComponentCreateTilemapCollidersType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS] = shovelerComponentTypeConfigurationOption("num_columns", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS] = shovelerComponentTypeConfigurationOption("num_rows", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS] = shovelerComponentTypeConfigurationOption("colliders", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ false, liveUpdateCollidersOption);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTilemapColliders, activateTilemapCollidersComponent, deactivateTilemapCollidersComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

const bool *shovelerComponentGetTilemapColliders(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTilemapColliders);

	return component->data;
}

static void *activateTilemapCollidersComponent(ShovelerComponent *component)
{
	int numColumns = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS);
	int numRows = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS);

	bool *colliders = malloc(numColumns * numRows * sizeof(bool));
	updateColliders(component, colliders);

	return colliders;
}

static void deactivateTilemapCollidersComponent(ShovelerComponent *component)
{
	bool *colliders = (bool *) component->data;

	free(colliders);
}

static void liveUpdateCollidersOption(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	bool *colliders = (bool *) component->data;
	assert(colliders != NULL);

	updateColliders(component, colliders);
}

static void updateColliders(ShovelerComponent *component, bool *colliders)
{
	int numColumns = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS);
	int numRows = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS);

	const unsigned char *collidersOption;
	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, &collidersOption, /* outputSize */ NULL);
	assert(collidersOption != NULL);

	for(int row = 0; row < numRows; ++row) {
		int rowIndex = row * numColumns;
		for(int column = 0; column < numColumns; ++column) {
			int columnIndex = rowIndex + column;

			colliders[columnIndex] = collidersOption[columnIndex];
		}
	}
}
