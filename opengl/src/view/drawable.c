#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/drawable.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddDrawable(ShovelerViewEntity *entity, const ShovelerViewDrawableConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNameDrawable)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateDrawableType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNameDrawable);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentDrawableOptionKeyType, configuration->type);
	if(configuration->type == SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES) {
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentDrawableOptionKeyTilesWidth, configuration->tilesWidth);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentDrawableOptionKeyTilesHeight, configuration->tilesHeight);
	}

	shovelerComponentActivate(component);
	return component;
}

ShovelerDrawable *shovelerViewEntityGetDrawable(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameDrawable);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetDrawableConfiguration(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameDrawable);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, shovelerComponentDrawableOptionKeyType);
	if(outputConfiguration->type == SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES) {
		outputConfiguration->tilesWidth = shovelerComponentGetConfigurationValueInt(component, shovelerComponentDrawableOptionKeyTilesWidth);
		outputConfiguration->tilesHeight = shovelerComponentGetConfigurationValueInt(component, shovelerComponentDrawableOptionKeyTilesHeight);
	}
	return true;
}

bool shovelerViewEntityUpdateDrawable(ShovelerViewEntity *entity, const ShovelerViewDrawableConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameDrawable);
	if(component == NULL) {
		shovelerLogWarning("Trying to update drawable of entity %lld which does not have a drawable, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentDrawableOptionKeyType, configuration->type);
	if(configuration->type == SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES) {
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentDrawableOptionKeyTilesWidth, configuration->tilesWidth);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentDrawableOptionKeyTilesHeight, configuration->tilesHeight);
	}
	return true;
}

bool shovelerViewEntityRemoveDrawable(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameDrawable);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove drawable from entity %lld which does not have a drawable, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNameDrawable);
}
