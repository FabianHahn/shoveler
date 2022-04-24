#ifndef SHOVELER_FILTER_DEPTH_TEXTURE_GAUSSIAN_H
#define SHOVELER_FILTER_DEPTH_TEXTURE_GAUSSIAN_H

#include <shoveler/filter.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerFilter* shovelerFilterDepthTextureGaussianCreate(
    struct ShovelerShaderCacheStruct* shaderCache,
    int width,
    int height,
    GLsizei samples,
    float exponentialFactor);

#endif
