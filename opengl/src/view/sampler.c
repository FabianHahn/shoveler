#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/sampler.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

bool shovelerViewEntityAddSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdSampler)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateSamplerType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdSampler);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE, configuration->interpolate);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS, configuration->useMipmaps);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP, configuration->clamp);

	shovelerComponentActivate(component);
	return component;
}

ShovelerSampler *shovelerViewEntityGetSampler(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSampler);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetSamplerConfiguration(ShovelerViewEntity *entity, ShovelerViewSamplerConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSampler);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->interpolate = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE);
	outputConfiguration->useMipmaps = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS);
	outputConfiguration->clamp = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP);
	return true;
}

bool shovelerViewEntityUpdateSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSampler);
	if(component == NULL) {
		shovelerLogWarning("Trying to update sampler of entity %lld which does not have a sampler, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE, configuration->interpolate);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS, configuration->useMipmaps);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP, configuration->clamp);
	return true;
}

bool shovelerViewEntityRemoveSampler(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSampler);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove sampler from entity %lld which does not have a sampler, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdSampler);
}
