#ifndef SHOVELER_TILESET_H
#define SHOVELER_TILESET_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

typedef struct ShovelerTilesetStruct {
	unsigned char columns;
	unsigned char rows;
	unsigned char padding;
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
} ShovelerTileset;

/** Creates a tileset from an existing image, with the caller retaining ownership over the passed image. */
ShovelerTileset *shovelerTilesetCreate(const ShovelerImage *image, unsigned char columns, unsigned char rows, unsigned char padding);
void shovelerTilesetFree(ShovelerTileset *tileset);

#endif
