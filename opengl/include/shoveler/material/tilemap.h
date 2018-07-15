#ifndef SHOVELER_MATERIAL_TILEMAP_H
#define SHOVELER_MATERIAL_TILEMAP_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

typedef struct {
	int height;
	int width;
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
} ShovelerMaterialTilemapTileset;

ShovelerMaterial *shovelerMaterialTilemapCreate(unsigned char width, unsigned char height);
/** adds a layer to a tilemap material, returning the index of the new layer */
int shovelerMaterialTilemapAddLayer(ShovelerMaterial *tilemapMaterial, ShovelerTexture *layerTexture);
/** adds a tileset to a tilemap material, returning the index of the new tileset */
int shovelerMaterialTilemapAddTileset(ShovelerMaterial *tilemapMaterial, ShovelerMaterialTilemapTileset tileset);

#endif
