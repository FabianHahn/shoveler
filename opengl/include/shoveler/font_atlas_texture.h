#ifndef SHOVELER_FONT_ATLAS_TEXTURE_H
#define SHOVELER_FONT_ATLAS_TEXTURE_H

#include <stdbool.h>

typedef struct ShovelerFontAtlasStruct ShovelerFontAtlas; // forward declaration: font_atlas.h
typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h

typedef struct ShovelerFontAtlasTextureStruct {
	ShovelerFontAtlas *fontAtlas;
	unsigned int atlasImageSize;
	ShovelerTexture *texture;
} ShovelerFontAtlasTexture;

ShovelerFontAtlasTexture *shovelerFontAtlasTextureCreate(ShovelerFontAtlas *fontAtlas);
/* Updates the font atlas texture, returning true if a new texture was generated. */
bool shovelerFontAtlasTextureUpdate(ShovelerFontAtlasTexture *fontAtlasTexture);
void shovelerFontAtlasTextureFree(ShovelerFontAtlasTexture *fontAtlasTexture);

#endif
