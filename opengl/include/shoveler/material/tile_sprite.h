#ifndef SHOVELER_MATERIAL_TILE_SPRITE_H
#define SHOVELER_MATERIAL_TILE_SPRITE_H

#include <stdbool.h> // bool

#include <shoveler/types.h>

typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerSpriteTileStruct ShovelerSpriteTile; // forward declaration: sprite/tile.h

ShovelerMaterial *shovelerMaterialTileSpriteCreate(ShovelerShaderCache *shaderCache, bool screenspace);
void shovelerMaterialTileSpriteSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize);
void shovelerMaterialTileSpriteSetActive(ShovelerMaterial *material, const ShovelerSpriteTile *spriteTile);

#endif
