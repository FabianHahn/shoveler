#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/tilemap_colliders.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTilemapColliders(ShovelerViewEntity *entity, const ShovelerViewTilemapCollidersConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdTilemapColliders)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTilemapColliders());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdTilemapColliders);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, configuration->colliders, configuration->numColumns * configuration->numRows);

	shovelerComponentActivate(component);
	return component;
}

const bool *shovelerViewEntityGetTilemapColliders(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapColliders);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTilemapCollidersConfiguration(ShovelerViewEntity *entity, ShovelerViewTilemapCollidersConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapColliders);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->numColumns = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS);
	outputConfiguration->numRows = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS);
	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, &outputConfiguration->colliders, /* outputSize */ NULL);

	return true;
}

bool shovelerViewEntityUpdateTilemapColliders(ShovelerViewEntity *entity, const ShovelerViewTilemapCollidersConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapColliders);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap colliders of entity %lld which does not have a tilemap colliders, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, configuration->numColumns);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, configuration->numRows);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, configuration->colliders, configuration->numColumns * configuration->numRows);
	return true;
}

bool shovelerViewEntityRemoveTilemapColliders(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTilemapColliders);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap colliders from entity %lld which does not have a tilemap colliders, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdTilemapColliders);
}
