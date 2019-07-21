#include <stdlib.h> // malloc free

#include "shoveler/font_atlas.h"
#include "shoveler/font_atlas_texture.h"
#include "shoveler/image.h"
#include "shoveler/texture.h"

ShovelerFontAtlasTexture *shovelerFontAtlasTextureCreate(ShovelerFontAtlas *fontAtlas)
{
	ShovelerFontAtlasTexture *fontAtlasTexture = malloc(sizeof(ShovelerFontAtlasTexture));
	fontAtlasTexture->fontAtlas = fontAtlas;
	fontAtlasTexture->atlasImageSize = 0;
	fontAtlasTexture->texture = NULL;

	return fontAtlasTexture;
}

void shovelerFontAtlasTextureUpdate(ShovelerFontAtlasTexture *fontAtlasTexture)
{
	if(fontAtlasTexture->texture == NULL || fontAtlasTexture->atlasImageSize != fontAtlasTexture->fontAtlas->image->width) {
		shovelerTextureFree(fontAtlasTexture->texture);
		fontAtlasTexture->atlasImageSize = fontAtlasTexture->fontAtlas->image->width;
		fontAtlasTexture->texture = shovelerTextureCreate2d(fontAtlasTexture->fontAtlas->image, false);
	}

	shovelerTextureUpdate(fontAtlasTexture->texture);
}

void shovelerFontAtlasTextureFree(ShovelerFontAtlasTexture *fontAtlasTexture)
{
	shovelerTextureFree(fontAtlasTexture->texture);
	free(fontAtlasTexture);
}
