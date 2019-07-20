#ifndef SHOVELER_MATERIAL_TEXTURE_H
#define SHOVELER_MATERIAL_TEXTURE_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialTextureCreate(ShovelerShaderCache *shaderCache, bool screenspace, ShovelerTexture *texture, bool manageTexture, ShovelerSampler *sampler, bool manageSampler);

#endif
