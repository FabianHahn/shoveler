#include "shoveler/view/tileset.h"

#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNameTileset)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTilesetType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNameTileset);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTilesetOptionKeyImageResource, configuration->imageResourceEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilesetOptionKeyNumColumns, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilesetOptionKeyNumRows, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilesetOptionKeyPadding, configuration->padding);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileset);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilesetConfiguration(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileset);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentTilesetOptionKeyImageResource);
	outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilesetOptionKeyNumColumns);
	outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilesetOptionKeyNumRows);
	outputConfiguration->padding = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTilesetOptionKeyPadding);
	return true;
}

bool shovelerViewEntityUpdateTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileset);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tileset of entity %lld which does not have a tileset, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTilesetOptionKeyImageResource, configuration->imageResourceEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilesetOptionKeyNumColumns, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilesetOptionKeyNumRows, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTilesetOptionKeyPadding, configuration->padding);
	return true;
}

bool shovelerViewEntityRemoveTileset(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileset);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tileset from entity %lld which does not have a tileset, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNameTileset);
}
