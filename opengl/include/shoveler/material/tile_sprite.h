#ifndef SHOVELER_MATERIAL_TILE_SPRITE_H
#define SHOVELER_MATERIAL_TILE_SPRITE_H

#include <shoveler/canvas.h>
#include <shoveler/material.h>
#include <shoveler/tileset.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialTileSpriteCreate(ShovelerShaderCache *shaderCache, bool screenspace);
void shovelerMaterialTileSpriteSetActive(ShovelerMaterial *material, const ShovelerCanvasTileSprite *tileSprite);

#endif
