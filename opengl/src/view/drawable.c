#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/drawable.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddDrawable(ShovelerViewEntity *entity, const ShovelerViewDrawableConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewDrawableComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateDrawableType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewDrawableComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, ShovelerComponentDrawableTypeOptionKey, configuration->type);
	if(configuration->type == SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES) {
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewDrawableTilesWidthOptionKey, configuration->tilesWidth);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewDrawableTilesHeightOptionKey, configuration->tilesHeight);
	}

	shovelerComponentActivate(component);
	return component;
}

ShovelerDrawable *shovelerViewEntityGetDrawable(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetDrawableConfiguration(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, ShovelerComponentDrawableTypeOptionKey);
	if(outputConfiguration->type == SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES) {
		outputConfiguration->tilesWidth = shovelerComponentGetConfigurationValueInt(component, shovelerViewDrawableTilesWidthOptionKey);
		outputConfiguration->tilesHeight = shovelerComponentGetConfigurationValueInt(component, shovelerViewDrawableTilesHeightOptionKey);
	}
	return true;
}

bool shovelerViewEntityUpdateDrawable(ShovelerViewEntity *entity, const ShovelerViewDrawableConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update drawable of entity %lld which does not have a drawable, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, ShovelerComponentDrawableTypeOptionKey, configuration->type);
	if(configuration->type == SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES) {
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewDrawableTilesWidthOptionKey, configuration->tilesWidth);
		shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewDrawableTilesHeightOptionKey, configuration->tilesHeight);
	}
	return true;
}

bool shovelerViewEntityRemoveDrawable(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove drawable from entity %lld which does not have a drawable, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewDrawableComponentTypeName);
}
