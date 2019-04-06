#ifndef SHOVELER_MATERIAL_TILEMAP_H
#define SHOVELER_MATERIAL_TILEMAP_H

#include <shoveler/material.h>
#include <shoveler/texture.h>
#include <shoveler/tilemap.h>
#include <shoveler/tileset.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialTilemapCreate(struct ShovelerShaderCacheStruct *shaderCache);
void shovelerMaterialTilemapSetActive(ShovelerMaterial *tilemapMaterial, ShovelerTilemap *tilemap);
void shovelerMaterialTilemapSetActiveTiles(ShovelerMaterial *tilemapMaterial, ShovelerTexture *tiles);
void shovelerMaterialTilemapSetActiveTileset(ShovelerMaterial *tilemapMaterial, int tilesetId, ShovelerTileset *tileset);

#endif
