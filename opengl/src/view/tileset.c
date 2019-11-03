#include "shoveler/view/tileset.h"

#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdTileset)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTilesetType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdTileset);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE_RESOURCE, configuration->imageResourceEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING, configuration->padding);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileset);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilesetConfiguration(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileset);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE_RESOURCE);
	outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS);
	outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS);
	outputConfiguration->padding = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING);
	return true;
}

bool shovelerViewEntityUpdateTileset(ShovelerViewEntity *entity, const ShovelerViewTilesetConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileset);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tileset of entity %lld which does not have a tileset, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE_RESOURCE, configuration->imageResourceEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING, configuration->padding);
	return true;
}

bool shovelerViewEntityRemoveTileset(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileset);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tileset from entity %lld which does not have a tileset, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdTileset);
}
