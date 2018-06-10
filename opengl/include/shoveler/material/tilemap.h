#ifndef SHOVELER_MATERIAL_TILEMAP_H
#define SHOVELER_MATERIAL_TILEMAP_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

ShovelerMaterial *shovelerMaterialTilemapCreate();
void shovelerMaterialTilemapSetSource(ShovelerMaterial *tilemapMaterial, ShovelerTexture *texture);
void shovelerMaterialTilemapSetTileset(ShovelerMaterial *tilemapMaterial, unsigned char tilesetId, ShovelerTexture *tilesetTexture, ShovelerSampler *tilesetSampler, unsigned char tilesetWidth, unsigned char tilesetHeight);

#endif
