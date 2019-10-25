#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/drawable/tiles.h"
#include "shoveler/view/drawable.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

static void *activateDrawableComponent(ShovelerComponent *component);
static void deactivateDrawableComponent(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddDrawable(ShovelerViewEntity *entity, const ShovelerViewDrawableConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewDrawableComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewDrawableComponentTypeName, activateDrawableComponent, deactivateDrawableComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewDrawableTypeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewDrawableTilesWidthOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewDrawableTilesHeightOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewDrawableComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewDrawableTypeOptionKey, configuration->type);
	if(configuration->type == SHOVELER_VIEW_DRAWABLE_TYPE_TILES) {
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

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, shovelerViewDrawableTypeOptionKey);
	if(outputConfiguration->type == SHOVELER_VIEW_DRAWABLE_TYPE_TILES) {
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

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewDrawableTypeOptionKey, configuration->type);
	if(configuration->type == SHOVELER_VIEW_DRAWABLE_TYPE_TILES) {
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


static void *activateDrawableComponent(ShovelerComponent *component)
{
	ShovelerViewDrawableType type = shovelerComponentGetConfigurationValueInt(component, shovelerViewDrawableTypeOptionKey);
	switch(type) {
		case SHOVELER_VIEW_DRAWABLE_TYPE_CUBE:
			return shovelerDrawableCubeCreate();
		case SHOVELER_VIEW_DRAWABLE_TYPE_QUAD:
			return shovelerDrawableQuadCreate();
		case SHOVELER_VIEW_DRAWABLE_TYPE_POINT:
			return shovelerDrawablePointCreate();
		case SHOVELER_VIEW_DRAWABLE_TYPE_TILES: {
			int tilesWidth = shovelerComponentGetConfigurationValueInt(component, shovelerViewDrawableTilesWidthOptionKey);
			int tilesHeight = shovelerComponentGetConfigurationValueInt(component, shovelerViewDrawableTilesHeightOptionKey);
			return shovelerDrawableTilesCreate(tilesWidth, tilesHeight);
		}
		default:
			shovelerLogWarning("Failed to activate drawable with unknown type %d.", type);
			return NULL;
	}
}

static void deactivateDrawableComponent(ShovelerComponent *component)
{
	shovelerDrawableFree(component->data);
}
