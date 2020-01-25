#ifndef SHOVELER_MATERIAL_TEXT_H
#define SHOVELER_MATERIAL_TEXT_H

#include <shoveler/material.h>
#include <shoveler/types.h>

typedef struct ShovelerFontAtlasTextureStruct ShovelerFontAtlasTexture; // forward declaration: font_atlas_texture.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialTextCreate(ShovelerShaderCache *shaderCache, bool screenspace);
void shovelerMaterialTextSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize);
void shovelerMaterialTextSetActiveFontAtlasTexture(ShovelerMaterial *material, ShovelerFontAtlasTexture *fontAtlasTexture);
void shovelerMaterialTextSetActiveText(ShovelerMaterial *material, const char *text, ShovelerVector2 corner, float size);
void shovelerMaterialTextSetActiveColor(ShovelerMaterial *material, ShovelerVector4 color);

#endif
