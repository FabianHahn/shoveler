#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/sampler.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

bool shovelerViewEntityAddSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNameSampler)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateSamplerType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNameSampler);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentSamplerOptionKeyInterpolate, configuration->interpolate);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentSamplerOptionKeyUseMipmaps, configuration->useMipmaps);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentSamplerOptionKeyClamp, configuration->clamp);

	shovelerComponentActivate(component);
	return component;
}

ShovelerSampler *shovelerViewEntityGetSampler(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameSampler);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetSamplerConfiguration(ShovelerViewEntity *entity, ShovelerViewSamplerConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameSampler);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->interpolate = shovelerComponentGetConfigurationValueBool(component, shovelerComponentSamplerOptionKeyInterpolate);
	outputConfiguration->useMipmaps = shovelerComponentGetConfigurationValueBool(component, shovelerComponentSamplerOptionKeyUseMipmaps);
	outputConfiguration->clamp = shovelerComponentGetConfigurationValueBool(component, shovelerComponentSamplerOptionKeyClamp);
	return true;
}

bool shovelerViewEntityUpdateSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameSampler);
	if(component == NULL) {
		shovelerLogWarning("Trying to update sampler of entity %lld which does not have a sampler, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentSamplerOptionKeyInterpolate, configuration->interpolate);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentSamplerOptionKeyUseMipmaps, configuration->useMipmaps);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentSamplerOptionKeyClamp, configuration->clamp);
	return true;
}

bool shovelerViewEntityRemoveSampler(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameSampler);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove sampler from entity %lld which does not have a sampler, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNameSampler);
}
