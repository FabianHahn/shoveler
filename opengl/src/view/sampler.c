#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/sampler.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

bool shovelerViewEntityAddSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewSamplerComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateSamplerType());
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
		shovelerLogWarning("Trying to update sampler of entity %lld which does not have a sampler, ignoring.", entity->id);
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
		shovelerLogWarning("Trying to remove sampler from entity %lld which does not have a sampler, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewSamplerComponentTypeName);
}
