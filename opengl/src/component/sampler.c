#include "shoveler/component/sampler.h"

#include <string.h> // strcmp

#include "shoveler/component.h"
#include "shoveler/sampler.h"

static void *activateSamplerComponent(ShovelerComponent *component);
static void deactivateSamplerComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateSamplerType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeNameSampler, activateSamplerComponent, deactivateSamplerComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentSamplerOptionKeyInterpolate, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentSamplerOptionKeyUseMipmaps, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentSamplerOptionKeyClamp, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerSampler *shovelerComponentGetSampler(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerComponentTypeNameSampler) == 0);

	return component->data;
}

static void  *activateSamplerComponent(ShovelerComponent *component)
{
	bool interpolate = shovelerComponentGetConfigurationValueBool(component, shovelerComponentSamplerOptionKeyInterpolate);
	bool useMipmaps = shovelerComponentGetConfigurationValueBool(component, shovelerComponentSamplerOptionKeyUseMipmaps);
	bool clamp = shovelerComponentGetConfigurationValueBool(component, shovelerComponentSamplerOptionKeyClamp);

	ShovelerSampler *sampler = shovelerSamplerCreate(interpolate, useMipmaps, clamp);
	return sampler;
}

static void deactivateSamplerComponent(ShovelerComponent *component)
{
	ShovelerSampler *sampler = (ShovelerSampler *) component->data;

	shovelerSamplerFree(sampler);
}
