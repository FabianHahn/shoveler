#ifndef SHOVELER_COMPONENT_SAMPLER_H
#define SHOVELER_COMPONENT_SAMPLER_H

typedef struct ShovelerSamplerStruct ShovelerSampler; // forward declaration: sampler.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameSampler = "sampler";
static const char *shovelerComponentSamplerOptionKeyInterpolate = "interpolate";
static const char *shovelerComponentSamplerOptionKeyUseMipmaps = "use_mipmaps";
static const char *shovelerComponentSamplerOptionKeyClamp = "clamp";

ShovelerComponentType *shovelerComponentCreateSamplerType();
ShovelerSampler *shovelerComponentGetSampler(ShovelerComponent *component);

#endif
