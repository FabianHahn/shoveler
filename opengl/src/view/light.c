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
	if(!shovelerViewHasComponentType(entity->view, shovelerViewLightComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateLightType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewLightComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewLightPositionOptionKey, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightTypeOptionKey, configuration->type);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightWidthOptionKey, configuration->width);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightHeightOptionKey, configuration->height);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightSamplesOptionKey, configuration->samples);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerViewLightAmbientFactorOptionKey, configuration->ambientFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerViewLightExponentialFactorOptionKey, configuration->exponentialFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewLightColorOptionKey, configuration->color);

	shovelerComponentActivate(component);
	return component;
}

ShovelerLight *shovelerViewEntityGetLight(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetLightConfiguration(ShovelerViewEntity *entity, ShovelerViewLightConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewLightPositionOptionKey);
	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightTypeOptionKey);
	outputConfiguration->width = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightWidthOptionKey);
	outputConfiguration->height = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightHeightOptionKey);
	outputConfiguration->samples = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightSamplesOptionKey);
	outputConfiguration->ambientFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerViewLightAmbientFactorOptionKey);
	outputConfiguration->exponentialFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerViewLightExponentialFactorOptionKey);
	outputConfiguration->color = shovelerComponentGetConfigurationValueVector3(component, shovelerViewLightColorOptionKey);
	return true;
}

bool shovelerViewEntityUpdateLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update light of entity %lld which does not have a light, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewLightPositionOptionKey, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightTypeOptionKey, configuration->type);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightWidthOptionKey, configuration->width);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightHeightOptionKey, configuration->height);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewLightSamplesOptionKey, configuration->samples);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerViewLightAmbientFactorOptionKey, configuration->ambientFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerViewLightExponentialFactorOptionKey, configuration->exponentialFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewLightColorOptionKey, configuration->color);
	return true;
}

bool shovelerViewEntityRemoveLight(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasScene(entity->view));

	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove light from entity %lld which does not have a light, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewLightComponentTypeName);
}
