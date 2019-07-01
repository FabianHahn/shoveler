#include <assert.h> // assert
#include <limits.h> // UINT_MAX
#include <stdlib.h> // malloc free

#include "shoveler/font.h"
#include "shoveler/font_atlas.h"
#include "shoveler/log.h"
#include "shoveler/image.h"

static void addBitmapToImage(ShovelerImage *image, FT_Bitmap bitmap, unsigned bottomLeftX, unsigned bottomLeftY);

ShovelerFontAtlas *shovelerFontAtlasCreate(ShovelerFont *font, unsigned int fontSize)
{
	FT_Error error = FT_Set_Pixel_Sizes(font->face, 0, fontSize);
	if(error != FT_Err_Ok) {
		shovelerLogError("Failed to set pixel size to %u for font '%s': %s", fontSize, font->name, FT_Error_String(error));
		return NULL;
	}

	error = FT_Load_Glyph(font->face, 0, FT_LOAD_RENDER);
	if(error != FT_Err_Ok) {
		shovelerLogError("Failed to load missing glyph for font '%s': %s", fontSize, font->name, FT_Error_String(error));
		return NULL;
	}

	ShovelerFontAtlas *fontAtlas = malloc(sizeof(ShovelerFontAtlas));
	fontAtlas->font = font;
	fontAtlas->fontSize = fontSize;
	fontAtlas->image = shovelerImageCreate(font->face->glyph->bitmap.width, font->face->glyph->bitmap.rows, 1);
	shovelerImageClear(fontAtlas->image);

	addBitmapToImage(fontAtlas->image, font->face->glyph->bitmap, 0, 0);

	return fontAtlas;
}

void shovelerFontAtlasFree(ShovelerFontAtlas *fontAtlas)
{
	if(fontAtlas == NULL) {
		return;
	}

	shovelerImageFree(fontAtlas->image);

	free(fontAtlas);
}

static void addBitmapToImage(ShovelerImage *image, FT_Bitmap bitmap, unsigned bottomLeftX, unsigned bottomLeftY)
{
	assert(bottomLeftX < image->width);
	assert(bottomLeftY < image->height);
	assert(UINT_MAX - bitmap.width >= image->width); // bitmap.width + image->width won't overflow
	assert(UINT_MAX - bitmap.rows >= image->height); // bitmap.rows + image->height won't overflow

	for(unsigned int bitmapX = 0; bitmapX < bitmap.width; bitmapX++) {
		unsigned int imageX = bottomLeftX + bitmapX;
		if(imageX >= image->width) {
			break;
		}

		for(unsigned int bitmapY = 0; bitmapY < bitmap.rows; bitmapY++) {
			unsigned int imageY = bottomLeftY + bitmapY;
			if(imageY >= image->height) {
				break;
			}

			shovelerImageGet(image, imageX, imageY, 0) = bitmap.buffer[(bitmap.rows - bitmapY - 1) * bitmap.pitch + bitmapX];
		}
	}
}
