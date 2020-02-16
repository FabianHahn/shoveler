#include "shoveler/view/canvas.h"

#include "shoveler/component.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdCanvas)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateCanvasType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdCanvas);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CANVAS_OPTION_ID_NUM_LAYERS, configuration->numLayers);

	shovelerComponentActivate(component);
	return component;
}

ShovelerCanvas *shovelerViewEntityGetCanvas(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdCanvas);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetCanvasConfiguration(ShovelerViewEntity *entity, ShovelerViewCanvasConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdCanvas);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->numLayers = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_CANVAS_OPTION_ID_NUM_LAYERS);
	return true;
}

bool shovelerViewEntityUpdateCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdCanvas);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas of entity %lld which does not have a canvas, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_CANVAS_OPTION_ID_NUM_LAYERS, configuration->numLayers);
	return true;
}

bool shovelerViewEntityRemoveCanvas(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdCanvas);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas from entity %lld which does not have a canvas, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdCanvas);
}
