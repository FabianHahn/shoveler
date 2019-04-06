#ifndef SHOVELER_MATERIAL_SCREENSPACE_TEXTURE_H
#define SHOVELER_MATERIAL_SCREENSPACE_TEXTURE_H

#include <stdbool.h> // bool

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialScreenspaceTextureCreate(struct ShovelerShaderCacheStruct *shaderCache, ShovelerTexture *texture, bool manageTexture, bool depthTexture, ShovelerSampler *sampler, bool manageSampler);

#endif
