#ifndef SHOVELER_MATERIAL_TILEMAP_H
#define SHOVELER_MATERIAL_TILEMAP_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

typedef struct {
	int tilemapWidth;
	int tilemapHeight;
	ShovelerTexture *tilemapTexture;
	int tilesetHeight;
	int tilesetWidth;
	int tilesetId;
	ShovelerTexture *tilesetTexture;
	ShovelerSampler *tilesetSampler;
} ShovelerMaterialTilemapLayer;

ShovelerMaterial *shovelerMaterialTilemapCreate();
/** adds a layer to a tilemap material, returning the index of the new layer */
int shovelerMaterialTilemapAddLayer(ShovelerMaterial *tilemapMaterial, ShovelerMaterialTilemapLayer layer);

#endif
