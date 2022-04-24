#ifndef SHOVELER_MATERIAL_TEXTURE_SPRITE_H
#define SHOVELER_MATERIAL_TEXTURE_SPRITE_H

#include <shoveler/types.h>
#include <stdbool.h> // bool

typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerSamplerStruct ShovelerSampler; // forward delcaration: sampler.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerTextureStruct ShovelerTexture; // forward delcaration: texture.h

typedef enum {
  SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_DEPTH,
  SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALPHA_MASK,
  SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALBEDO
} ShovelerMaterialTextureSpriteType;

ShovelerMaterial* shovelerMaterialTextureSpriteCreate(
    ShovelerShaderCache* shaderCache, bool screenspace, ShovelerMaterialTextureSpriteType type);
void shovelerMaterialTextureSpriteSetActiveRegion(
    ShovelerMaterial* material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize);
void shovelerMaterialTextureSpriteSetActiveSprite(
    ShovelerMaterial* material, ShovelerVector2 position, ShovelerVector2 size);
void shovelerMaterialTextureSpriteSetColor(ShovelerMaterial* material, ShovelerVector4 color);
void shovelerMaterialTextureSpriteSetActiveTexture(
    ShovelerMaterial* material, ShovelerTexture* texture, ShovelerSampler* sampler);

#endif
