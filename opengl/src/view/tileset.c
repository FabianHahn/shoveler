#include "shoveler/view/tileset.h"

#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTilesetComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTilesetType());
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
		shovelerLogWarning("Trying to update tileset of entity %lld which does not have a tileset, ignoring.", entity->id);
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
		shovelerLogWarning("Trying to remove tileset from entity %lld which does not have a tileset, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilesetComponentTypeName);
}
