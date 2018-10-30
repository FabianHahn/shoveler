#ifndef SHOVELER_SAMPLER_H
#define SHOVELER_SAMPLER_H

#include <stdbool.h> // bool

#include <glad/glad.h>

typedef struct {
	GLuint sampler;
} ShovelerSampler;

ShovelerSampler *shovelerSamplerCreate(bool interpolate, bool useMipmaps, bool clamp);
bool shovelerSamplerUse(ShovelerSampler *sampler, GLuint unit);
void shovelerSamplerFree(ShovelerSampler *shader);

#endif
