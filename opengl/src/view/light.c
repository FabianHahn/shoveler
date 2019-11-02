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
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentLightOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeyType, configuration->type);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeyWidth, configuration->width);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeyHeight, configuration->height);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeySamples, configuration->samples);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerComponentLightOptionKeyAmbientFactor, configuration->ambientFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerComponentLightOptionKeyExponentialFactor, configuration->exponentialFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerComponentLightOptionKeyColor, configuration->color);

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

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentLightOptionKeyPosition);
	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeyType);
	outputConfiguration->width = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeyWidth);
	outputConfiguration->height = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeyHeight);
	outputConfiguration->samples = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeySamples);
	outputConfiguration->ambientFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerComponentLightOptionKeyAmbientFactor);
	outputConfiguration->exponentialFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerComponentLightOptionKeyExponentialFactor);
	outputConfiguration->color = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentLightOptionKeyColor);
	return true;
}

bool shovelerViewEntityUpdateLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdLight);
	if(component == NULL) {
		shovelerLogWarning("Trying to update light of entity %lld which does not have a light, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentLightOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeyType, configuration->type);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeyWidth, configuration->width);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeyHeight, configuration->height);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentLightOptionKeySamples, configuration->samples);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerComponentLightOptionKeyAmbientFactor, configuration->ambientFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerComponentLightOptionKeyExponentialFactor, configuration->exponentialFactor);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerComponentLightOptionKeyColor, configuration->color);
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
