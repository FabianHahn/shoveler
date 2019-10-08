#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/tilemap_colliders.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

static void *activateTilemapCollidersComponent(ShovelerComponent *component);
static void deactivateTilemapCollidersComponent(ShovelerComponent *component);
static void liveUpdateCollidersOption(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateColliders(ShovelerComponent *component, bool *colliders);

ShovelerComponent *shovelerViewEntityAddTilemapColliders(ShovelerViewEntity *entity, const ShovelerViewTilemapCollidersConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTilemapCollidersComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilemapCollidersComponentTypeName, activateTilemapCollidersComponent, deactivateTilemapCollidersComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapCollidersNumColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapCollidersNumRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilemapCollidersCollidersOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ false, liveUpdateCollidersOption);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapCollidersComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapCollidersNumColumnsOptionKey, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapCollidersNumRowsOptionKey, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapCollidersCollidersOptionKey, configuration->colliders, configuration->numColumns * configuration->numRows);

	shovelerComponentActivate(component);
	return component;
}

const bool *shovelerViewEntityGetTilemapColliders(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapCollidersComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilemapCollidersConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapCollidersConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapCollidersComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapCollidersNumColumnsOptionKey);
	outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilemapCollidersNumRowsOptionKey);
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewTilemapCollidersCollidersOptionKey, &outputConfiguration->colliders, /* outputSize */ NULL);

	return true;
}

bool shovelerViewEntityUpdateTilemapColliders(ShovelerViewEntity *entity, const ShovelerViewTilemapCollidersConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapCollidersComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap colliders of entity %lld which does not have a tilemap colliders, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapCollidersNumColumnsOptionKey, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilemapCollidersNumRowsOptionKey, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewTilemapCollidersCollidersOptionKey, configuration->colliders, configuration->numColumns * configuration->numRows);
	return true;
}

bool shovelerViewEntityRemoveTilemapColliders(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapCollidersComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap colliders from entity %lld which does not have a tilemap colliders, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapCollidersComponentTypeName);
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
	ShovelerTexture *texture = (ShovelerTexture *) component->data;

	shovelerTextureFree(texture);
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
