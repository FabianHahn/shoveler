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

typedef struct {
	ShovelerLight *light;
	ShovelerViewComponentCallback *positionCallback;
} LightComponentData;

static void *activateLightComponent(ShovelerComponent *component);
static void deactivateLightComponent(ShovelerComponent *component);
static void updateLightPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

ShovelerComponent *shovelerViewEntityAddLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewLightComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewLightComponentTypeName, activateLightComponent, deactivateLightComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewLightPositionOptionKey, shovelerViewPositionComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateLightPositionDependency);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightTypeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightWidthOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightHeightOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightSamplesOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightAmbientFactorOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightExponentialFactorOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightColorOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
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

static void *activateLightComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasScene(component->entity->view));
	assert(shovelerViewHasShaderCache(component->entity->view));

	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewLightPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);
	assert(positionCoordinates != NULL);

	ShovelerViewLightType type = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightTypeOptionKey);
	ShovelerLight *light;
	switch(type) {
		case SHOVELER_VIEW_LIGHT_TYPE_SPOT:
			shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
			return NULL;
		case SHOVELER_VIEW_LIGHT_TYPE_POINT: {
			ShovelerShaderCache *shaderCache = shovelerViewGetShaderCache(component->entity->view);

			int width = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightWidthOptionKey);
			int height = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightHeightOptionKey);
			int samples = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightSamplesOptionKey);
			float ambientFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerViewLightAmbientFactorOptionKey);
			float exponentialFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerViewLightExponentialFactorOptionKey);
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, shovelerViewLightColorOptionKey);

			light = shovelerLightPointCreate(shaderCache, *positionCoordinates, width, height, samples, ambientFactor, exponentialFactor, color);
		} break;
		default:
			shovelerLogWarning("Trying to create light with unknown light type %d, ignoring.", type);
			return NULL;
	}

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneAddLight(scene, light);
	return light;
}

static void deactivateLightComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasScene(component->entity->view));
	assert(shovelerViewHasShaderCache(component->entity->view));

	ShovelerLight *light = (ShovelerLight *) component->data;

	ShovelerShaderCache *shaderCache = shovelerViewGetShaderCache(component->entity->view);
	shovelerShaderCacheInvalidateLight(shaderCache, light);

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneRemoveLight(scene, light);
}

static void updateLightPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerLight *light = (ShovelerLight *) component->data;

	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewLightPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);
	assert(positionCoordinates != NULL);

	shovelerLightUpdatePosition(light, *positionCoordinates);
}
