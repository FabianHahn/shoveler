#ifndef SHOVELER_SPRITE_TEXTURE_H
#define SHOVELER_SPRITE_TEXTURE_H

#include <shoveler/sprite.h>

typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h

typedef struct ShovelerSpriteTextureStruct {
	ShovelerSprite sprite;
	ShovelerTileset *tileset;
} ShovelerSpriteTexture;

ShovelerSprite *shovelerSpriteTextureCreate(ShovelerMaterial *material, ShovelerTexture *texture);

#endif
