#ifndef SHOVELER_MATERIAL_TILE_SPRITE_H
#define SHOVELER_MATERIAL_TILE_SPRITE_H

#include <shoveler/canvas.h>
#include <shoveler/material.h>
#include <shoveler/tileset.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialTileSpriteCreate(struct ShovelerShaderCacheStruct *shaderCache);
void shovelerMaterialTileSpriteSetActive(ShovelerMaterial *material, const ShovelerCanvasTileSprite *tileSprite);

#endif
