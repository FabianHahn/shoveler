#ifndef SHOVELER_MATERIAL_TILE_SPRITE_H
#define SHOVELER_MATERIAL_TILE_SPRITE_H

#include <shoveler/material.h>
#include <shoveler/tileset.h>

typedef struct {
	int tilesetColumn;
	int tilesetRow;
	float positionX;
	float positionZ;
	float width;
	float height;
} ShovelerMaterialTileSpriteConfiguration;

ShovelerMaterial *shovelerMaterialTileSpriteCreate(ShovelerTileset *tileset, ShovelerMaterialTileSpriteConfiguration configuration);

#endif
