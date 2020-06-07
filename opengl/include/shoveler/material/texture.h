#ifndef SHOVELER_MATERIAL_TEXTURE_H
#define SHOVELER_MATERIAL_TEXTURE_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

typedef enum {
	SHOVELER_MATERIAL_TEXTURE_TYPE_DEPTH,
	SHOVELER_MATERIAL_TEXTURE_TYPE_ALBEDO,
	SHOVELER_MATERIAL_TEXTURE_TYPE_PHONG,
} ShovelerMaterialTextureType;

ShovelerMaterial *shovelerMaterialTextureCreate(ShovelerShaderCache *shaderCache, bool screenspace, ShovelerMaterialTextureType type, ShovelerTexture *texture, bool manageTexture, ShovelerSampler *sampler, bool manageSampler);

#endif
