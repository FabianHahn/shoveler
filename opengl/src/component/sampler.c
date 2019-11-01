#include "shoveler/component/sampler.h"

#include <string.h> // strcmp

#include "shoveler/component.h"
#include "shoveler/sampler.h"

static void *activateSamplerComponent(ShovelerComponent *component);
static void deactivateSamplerComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateSamplerType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewSamplerComponentTypeName, activateSamplerComponent, deactivateSamplerComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewSamplerInterpolateOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewSamplerUseMipmapsOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewSamplerClampOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerSampler *shovelerComponentGetSampler(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewSamplerComponentTypeName) == 0);

	return component->data;
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
