#ifndef SHOVELER_SPRITE_TEXTURE_H
#define SHOVELER_SPRITE_TEXTURE_H

#include <shoveler/sprite.h>

typedef struct ShovelerSamplerStruct ShovelerSampler; // forward declaration: sampler.h
typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h

typedef struct ShovelerSpriteTextureStruct {
	ShovelerSprite sprite;
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
} ShovelerSpriteTexture;

/**
 * Create a texture sprite.
 *
 * The passed material must be of type texture sprite. The caller retains ownership over the passed
 * material and sampler.
 */
ShovelerSprite *shovelerSpriteTextureCreate(ShovelerMaterial *material, ShovelerTexture *texture, ShovelerSampler *sampler);

#endif
