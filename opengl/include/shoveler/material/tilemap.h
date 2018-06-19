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
} ShovelerMaterialTilemapLayerData;

ShovelerMaterial *shovelerMaterialTilemapCreate();
ShovelerMaterial *shovelerMaterialTilemapCreateLayer(ShovelerMaterial *tilemapMaterial, ShovelerMaterialTilemapLayerData layerData);

#endif
