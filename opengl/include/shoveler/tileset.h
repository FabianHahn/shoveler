#ifndef SHOVELER_TILESET_H
#define SHOVELER_TILESET_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

typedef struct {
	unsigned char columns;
	unsigned char rows;
	unsigned char padding;
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
} ShovelerTileset;

ShovelerTileset *shovelerTilesetCreate(ShovelerImage *image, unsigned char columns, unsigned char rows, unsigned char padding);
void shovelerTilesetFree(ShovelerTileset *tileset);

#endif
