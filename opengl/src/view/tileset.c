#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/resources.h"
#include "shoveler/view/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

static void *activateTilesetComponent(ShovelerComponent *component);
static void deactivateTilesetComponent(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTilesetComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilesetComponentTypeName, activateTilesetComponent, deactivateTilesetComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilesetImageResourceOptionKey, shovelerViewResourceComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilesetNumColumnsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilesetNumRowsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTilesetPaddingOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTilesetComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilesetImageResourceOptionKey, configuration->imageResourceEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilesetNumColumnsOptionKey, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilesetNumRowsOptionKey, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilesetPaddingOptionKey, configuration->padding);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilesetConfiguration(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilesetImageResourceOptionKey);
	outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilesetNumColumnsOptionKey);
	outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilesetNumRowsOptionKey);
	outputConfiguration->padding = shovelerComponentGetConfigurationValueInt(component, shovelerViewTilesetPaddingOptionKey);
	return true;
}

bool shovelerViewEntityUpdateTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tileset of entity %lld which does not have a tileset, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTilesetImageResourceOptionKey, configuration->imageResourceEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilesetNumColumnsOptionKey, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilesetNumRowsOptionKey, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTilesetPaddingOptionKey, configuration->padding);
	return true;
}

bool shovelerViewEntityRemoveTileset(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tileset from entity %lld which does not have a tileset, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilesetComponentTypeName);
}

static void *activateTilesetComponent(ShovelerComponent *component)
{
	long long int imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTilesetImageResourceOptionKey);
	ShovelerViewEntity *imageResourceEntity = shovelerViewGetEntity(component->entity->view, imageResourceEntityId);
	assert(imageResourceEntity != NULL);
	ShovelerImage *image = shovelerViewEntityGetResource(imageResourceEntity);
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
