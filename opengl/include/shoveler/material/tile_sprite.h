#ifndef SHOVELER_MATERIAL_TILE_SPRITE_H
#define SHOVELER_MATERIAL_TILE_SPRITE_H

#include <shoveler/canvas.h>
#include <shoveler/material.h>
#include <shoveler/tileset.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerSpriteTileStruct ShovelerSpriteTile; // forward declaration: sprite/tile.h

ShovelerMaterial *shovelerMaterialTileSpriteCreate(ShovelerShaderCache *shaderCache, bool screenspace);
void shovelerMaterialTileSpriteSetActiveLegacy(ShovelerMaterial *material, const ShovelerCanvasTileSprite *tileSprite);
void shovelerMaterialTileSpriteSetActive(ShovelerMaterial *material, const ShovelerSpriteTile *spriteTile);

#endif
