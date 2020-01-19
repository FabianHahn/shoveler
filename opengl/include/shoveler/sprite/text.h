#ifndef SHOVELER_SPRITE_TEXT_H
#define SHOVELER_SPRITE_TEXT_H

#include <shoveler/sprite.h>

typedef struct ShovelerFontAtlasTextureStruct ShovelerFontAtlasTexture; // forward declaration: font_atlas_texture.h

typedef struct ShovelerSpriteTextStruct {
	ShovelerSprite sprite;
	ShovelerFontAtlasTexture *fontAtlasTexture;
	const char *content;
	bool isContentManaged;
	float fontSize;
	ShovelerVector4 color;
} ShovelerSpriteText;

ShovelerSprite *shovelerSpriteTextCreate(ShovelerFontAtlasTexture *fontAtlasTexture, float fontSize, ShovelerVector4 color);
void shovelerSpriteTextSetContent(ShovelerSprite *sprite, const char *content, bool copyContent);

#endif
