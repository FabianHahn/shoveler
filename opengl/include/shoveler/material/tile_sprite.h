#ifndef SHOVELER_MATERIAL_TILE_SPRITE_H
#define SHOVELER_MATERIAL_TILE_SPRITE_H

#include <shoveler/types.h>
#include <stdbool.h> // bool

typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerSpriteTileStruct ShovelerSpriteTile; // forward declaration: sprite/tile.h
typedef struct ShovelerTilesetStruct ShovelerTileset; // forward delcaration: tileset.h

ShovelerMaterial* shovelerMaterialTileSpriteCreate(
    ShovelerShaderCache* shaderCache, bool screenspace);
void shovelerMaterialTileSpriteSetActiveRegion(
    ShovelerMaterial* material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize);
void shovelerMaterialTileSpriteSetActiveTile(
    ShovelerMaterial* material, int tilesetRow, int tilesetColumn);
void shovelerMaterialTileSpriteSetActiveTileset(
    ShovelerMaterial* material, ShovelerTileset* tileset);
void shovelerMaterialTileSpriteSetActiveSprite(
    ShovelerMaterial* material, ShovelerVector2 position, ShovelerVector2 size);
void shovelerMaterialTileSpriteSetActive(
    ShovelerMaterial* material, const ShovelerSpriteTile* spriteTile);

#endif
