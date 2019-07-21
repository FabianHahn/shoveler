#ifndef SHOVELER_TILESET_H
#define SHOVELER_TILESET_H

#include <stdbool.h> // bool

typedef struct ShovelerImageStruct ShovelerImage; // forward declaration: image.h
typedef struct ShovelerSamplerStruct ShovelerSampler; // forward declaration: sampler.h
typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h

typedef struct ShovelerTilesetStruct {
	unsigned char columns;
	unsigned char rows;
	unsigned char padding;
	bool manageTexture;
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
} ShovelerTileset;

/** Creates a tileset from an existing image, with the caller retaining ownership over the passed image. */
ShovelerTileset *shovelerTilesetCreate(const ShovelerImage *image, unsigned char columns, unsigned char rows, unsigned char padding);
/** Creates a tileset from an existing texture, with the caller retaining ownership over the passed texture. */
ShovelerTileset *shovelerTilesetCreateFromTexture(ShovelerTexture *texture, unsigned char columns, unsigned char rows, unsigned char padding);
void shovelerTilesetFree(ShovelerTileset *tileset);

#endif
