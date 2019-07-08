#ifndef SHOVELER_MATERIAL_TEXT_H
#define SHOVELER_MATERIAL_TEXT_H

#include <shoveler/material.h>
#include <shoveler/types.h>

typedef struct ShovelerFontAtlasTextureStruct ShovelerFontAtlasTexture; // forward declaration: font_atlas_texture.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialTextCreate(ShovelerShaderCache *shaderCache);
void shovelerMaterialTextSetActiveFontAtlasTexture(ShovelerMaterial *material, ShovelerFontAtlasTexture *fontAtlasTexture);
void shovelerMaterialTextSetActiveText(ShovelerMaterial *material, const char *text, ShovelerVector2 corner, float size);

#endif
