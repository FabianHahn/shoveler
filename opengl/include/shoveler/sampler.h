#ifndef SHOVELER_SAMPLER_H
#define SHOVELER_SAMPLER_H

#include <glad/glad.h>
#include <stdbool.h> // bool

typedef struct ShovelerSamplerStruct {
  GLuint sampler;
} ShovelerSampler;

ShovelerSampler* shovelerSamplerCreate(bool interpolate, bool useMipmaps, bool clamp);
bool shovelerSamplerUse(ShovelerSampler* sampler, GLuint unit);
void shovelerSamplerFree(ShovelerSampler* shader);

#endif
