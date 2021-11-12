#ifndef SHOVELER_COMPONENT_SAMPLER_H
#define SHOVELER_COMPONENT_SAMPLER_H

typedef struct ShovelerSamplerStruct ShovelerSampler; // forward declaration: sampler.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdSampler;

typedef enum {
	SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE,
	SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS,
	SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP,
} ShovelerComponentSamplerOptionId;

ShovelerComponentType *shovelerComponentCreateSamplerType();
ShovelerSampler *shovelerComponentGetSampler(ShovelerComponent *component);

#endif
