#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/sampler.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

static void *activateSamplerComponent(ShovelerComponent *component);
static void deactivateSamplerComponent(ShovelerComponent *component);

bool shovelerViewEntityAddSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewSamplerComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewSamplerComponentTypeName, activateSamplerComponent, deactivateSamplerComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewSamplerInterpolateOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewSamplerUseMipmapsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewSamplerClampOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewSamplerComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewSamplerInterpolateOptionKey, configuration->interpolate);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewSamplerUseMipmapsOptionKey, configuration->useMipmaps);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewSamplerClampOptionKey, configuration->clamp);

	shovelerComponentActivate(component);
	return component;
}

ShovelerSampler *shovelerViewEntityGetSampler(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewSamplerComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetSamplerConfiguration(ShovelerViewEntity *entity, ShovelerViewSamplerConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewSamplerComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->interpolate = shovelerComponentGetConfigurationValueBool(component, shovelerViewSamplerInterpolateOptionKey);
	outputConfiguration->useMipmaps = shovelerComponentGetConfigurationValueBool(component, shovelerViewSamplerUseMipmapsOptionKey);
	outputConfiguration->clamp = shovelerComponentGetConfigurationValueBool(component, shovelerViewSamplerClampOptionKey);
	return true;
}

bool shovelerViewEntityUpdateSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewSamplerComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update sampler of entity %lld which does not have a sampler, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewSamplerInterpolateOptionKey, configuration->interpolate);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewSamplerUseMipmapsOptionKey, configuration->useMipmaps);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewSamplerClampOptionKey, configuration->clamp);
	return true;
}

bool shovelerViewEntityRemoveSampler(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewSamplerComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove sampler from entity %lld which does not have a sampler, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewSamplerComponentTypeName);
}

static void  *activateSamplerComponent(ShovelerComponent *component)
{
	bool interpolate = shovelerComponentGetConfigurationValueBool(component, shovelerViewSamplerInterpolateOptionKey);
	bool useMipmaps = shovelerComponentGetConfigurationValueBool(component, shovelerViewSamplerUseMipmapsOptionKey);
	bool clamp = shovelerComponentGetConfigurationValueBool(component, shovelerViewSamplerClampOptionKey);

	ShovelerSampler *sampler = shovelerSamplerCreate(interpolate, useMipmaps, clamp);
	return sampler;
}

static void deactivateSamplerComponent(ShovelerComponent *component)
{
	ShovelerSampler *sampler = (ShovelerSampler *) component->data;

	shovelerSamplerFree(sampler);
}
