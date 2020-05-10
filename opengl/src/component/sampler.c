#include "shoveler/component/sampler.h"

#include "shoveler/component.h"
#include "shoveler/sampler.h"

const char *const shovelerComponentTypeIdSampler = "sampler";

static void *activateSamplerComponent(ShovelerComponent *component);
static void deactivateSamplerComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateSamplerType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE] = shovelerComponentTypeConfigurationOption("interpolate", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS] = shovelerComponentTypeConfigurationOption("use_mipmaps", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP] = shovelerComponentTypeConfigurationOption("clamp", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdSampler, activateSamplerComponent, /* update */ NULL, deactivateSamplerComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerSampler *shovelerComponentGetSampler(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdSampler);

	return component->data;
}

static void  *activateSamplerComponent(ShovelerComponent *component)
{
	bool interpolate = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE);
	bool useMipmaps = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS);
	bool clamp = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP);

	ShovelerSampler *sampler = shovelerSamplerCreate(interpolate, useMipmaps, clamp);
	return sampler;
}

static void deactivateSamplerComponent(ShovelerComponent *component)
{
	ShovelerSampler *sampler = (ShovelerSampler *) component->data;

	shovelerSamplerFree(sampler);
}
