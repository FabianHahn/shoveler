#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <shoveler/view.h>

#include "shoveler/light/point.h"
#include "shoveler/view/light.h"
#include "shoveler/view/position.h"
#include "shoveler/view/scene.h"
#include "shoveler/view/shader_cache.h"
#include "shoveler/component.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/scene.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdLight)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateLightType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdLight);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_TYPE, configuration->type);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_WIDTH, configuration->width);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_HEIGHT, configuration->height);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_SAMPLES, configuration->samples);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_AMBIENT_FACTOR, configuration->ambientFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_EXPONENTIAL_FACTOR, configuration->exponentialFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_COLOR, configuration->color);

	shovelerComponentActivate(component);
	return component;
}

ShovelerLight *shovelerViewEntityGetLight(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdLight);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetLightConfiguration(ShovelerViewEntity *entity, ShovelerViewLightConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdLight);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION);
	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_TYPE);
	outputConfiguration->width = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_WIDTH);
	outputConfiguration->height = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_HEIGHT);
	outputConfiguration->samples = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_SAMPLES);
	outputConfiguration->ambientFactor = shovelerComponentGetConfigurationValueFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_AMBIENT_FACTOR);
	outputConfiguration->exponentialFactor = shovelerComponentGetConfigurationValueFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_EXPONENTIAL_FACTOR);
	outputConfiguration->color = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_COLOR);
	return true;
}

bool shovelerViewEntityUpdateLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdLight);
	if(component == NULL) {
		shovelerLogWarning("Trying to update light of entity %lld which does not have a light, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_TYPE, configuration->type);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_WIDTH, configuration->width);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_HEIGHT, configuration->height);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_SAMPLES, configuration->samples);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_AMBIENT_FACTOR, configuration->ambientFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_EXPONENTIAL_FACTOR, configuration->exponentialFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_COLOR, configuration->color);
	return true;
}

bool shovelerViewEntityRemoveLight(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasScene(entity->view));

	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdLight);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove light from entity %lld which does not have a light, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdLight);
}
