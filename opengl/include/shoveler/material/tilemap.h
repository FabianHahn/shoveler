#ifndef SHOVELER_MATERIAL_TILEMAP_H
#define SHOVELER_MATERIAL_TILEMAP_H

#include <shoveler/types.h>

typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerSpriteTilemapStruct ShovelerSpriteTilemap; // forward declaration: sprite/tilemap.h
typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerTilemapStruct ShovelerTilemap; // forward declaration: tilemap.h
typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h

ShovelerMaterial *shovelerMaterialTilemapCreate(ShovelerShaderCache *shaderCache, bool screenspace);
void shovelerMaterialTilemapSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize);
void shovelerMaterialTilemapSetActiveSprite(ShovelerMaterial *tilemapMaterial, ShovelerSpriteTilemap *sprite);
void shovelerMaterialTilemapSetActive(ShovelerMaterial *tilemapMaterial, ShovelerTilemap *tilemap);
void shovelerMaterialTilemapSetActiveTiles(ShovelerMaterial *tilemapMaterial, ShovelerTexture *tiles);
void shovelerMaterialTilemapSetActiveTileset(ShovelerMaterial *tilemapMaterial, int tilesetId, ShovelerTileset *tileset);

#endif
