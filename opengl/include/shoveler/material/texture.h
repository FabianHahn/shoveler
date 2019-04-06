#ifndef SHOVELER_MATERIAL_TEXTURE_H
#define SHOVELER_MATERIAL_TEXTURE_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialTextureCreate(struct ShovelerShaderCacheStruct *shaderCache, ShovelerTexture *texture, bool manageTexture, ShovelerSampler *sampler, bool manageSampler);

#endif
