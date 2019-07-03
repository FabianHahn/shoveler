#include <assert.h> // assert
#include <limits.h> // UINT_MAX
#include <stdlib.h> // malloc free

#include "shoveler/font.h"
#include "shoveler/font_atlas.h"
#include "shoveler/log.h"
#include "shoveler/image.h"

static void insertGlyph(ShovelerFontAtlas *fontAtlas, FT_Bitmap bitmap);
static void growImage(ShovelerFontAtlas *fontAtlas);
static void addBitmapToImage(ShovelerImage *image, FT_Bitmap bitmap, unsigned bottomLeftX, unsigned bottomLeftY);
static ShovelerFontAtlasRectangle *allocateRectangle(unsigned int minX, unsigned int minY, unsigned int width, unsigned int height);

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
	fontAtlas->image = shovelerImageCreate(64, 64, 1);
	shovelerImageClear(fontAtlas->image);

	fontAtlas->skylineEdges = g_queue_new();
	g_queue_push_tail(fontAtlas->skylineEdges, allocateRectangle(0, 0, 64, 0));

	insertGlyph(fontAtlas, font->face->glyph->bitmap);
	FT_Load_Char(font->face, 'a', FT_LOAD_RENDER);
	insertGlyph(fontAtlas, font->face->glyph->bitmap);
	FT_Load_Char(font->face, 'b', FT_LOAD_RENDER);
	insertGlyph(fontAtlas, font->face->glyph->bitmap);
	FT_Load_Char(font->face, 'c', FT_LOAD_RENDER);
	insertGlyph(fontAtlas, font->face->glyph->bitmap);

	return fontAtlas;
}

void shovelerFontAtlasFree(ShovelerFontAtlas *fontAtlas)
{
	if(fontAtlas == NULL) {
		return;
	}

	g_queue_free_full(fontAtlas->skylineEdges, free);
	shovelerImageFree(fontAtlas->image);

	free(fontAtlas);
}

static void insertGlyph(ShovelerFontAtlas *fontAtlas, FT_Bitmap bitmap)
{
	GList *bestStartSkylineEdgeIter = NULL;
	GList *bestEndSkylineEdgeIter = NULL;
	unsigned int bestHeight = UINT_MAX;
	bool isRotated = false;
	for(GList *iter = fontAtlas->skylineEdges->head; iter != NULL; iter = iter->next) {
		ShovelerFontAtlasRectangle *skylineEdge = iter->data;

		// try to put the bitmap on top of this skyline edge
		unsigned int candidateHeight = skylineEdge->height + bitmap.rows;
		if(candidateHeight < bestHeight && candidateHeight <= fontAtlas->image->height) {
			// the bitmap could be wider than this skyline edge, so we might have to extend across a few more as long as their height is lower
			GList *endIter = iter;
			unsigned int currentEdgeWidth = skylineEdge->width;
			while(currentEdgeWidth < bitmap.width && endIter != NULL) {
				endIter = endIter->next;
				ShovelerFontAtlasRectangle *additionalSkylineEdge = endIter->data;

				if(additionalSkylineEdge->height > skylineEdge->height) {
					// this skyline edge is higher than the one we started with, but the bitmap doesn't fit yet - abort
					break;
				}

				currentEdgeWidth += additionalSkylineEdge->width;
			}

			if(currentEdgeWidth >= bitmap.width) {
				// we could put the bitmap here, and this is the best we've found so far
				bestStartSkylineEdgeIter = iter;
				bestEndSkylineEdgeIter = endIter;
				bestHeight = candidateHeight;
				isRotated = false;
			}
		}

		// todo try rotated as well
	}

	if(bestStartSkylineEdgeIter == NULL) {
		// doesn't fit, grow and retry
		growImage(fontAtlas);
		// insertGlyph(fontAtlas, bitmap);
		return;
	}

	ShovelerFontAtlasRectangle *bestStartSkylineEdge = bestStartSkylineEdgeIter->data;

	// place the bitmap here
	addBitmapToImage(fontAtlas->image, bitmap, bestStartSkylineEdge->minX, bestStartSkylineEdge->height);
	
	GList *currentSkylineEdgeIter = bestStartSkylineEdgeIter;
	unsigned int remainingWidth = bitmap.width;
	for(GList *iter = bestStartSkylineEdgeIter; iter != bestEndSkylineEdgeIter; iter = iter->next) {
		ShovelerFontAtlasRectangle *currentSkylineEdge = iter->data;

		// lift it up to the new height
		currentSkylineEdge->height = bestHeight;

		// update remaining width
		remainingWidth -= currentSkylineEdge->width;
	}

	// split the last edge into two
	ShovelerFontAtlasRectangle *bestEndSkylineEdge = bestEndSkylineEdgeIter->data;
	
	ShovelerFontAtlasRectangle* splitEndSkylineEdge = allocateRectangle(bestEndSkylineEdge->minX + remainingWidth, 0, bestEndSkylineEdge->width - remainingWidth, bestEndSkylineEdge->height);
	g_queue_insert_after(fontAtlas->skylineEdges, bestEndSkylineEdgeIter, splitEndSkylineEdge);

	bestEndSkylineEdge->width = remainingWidth;
	bestEndSkylineEdge->height = bestHeight;
}

static void growImage(ShovelerFontAtlas *fontAtlas)
{

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

static ShovelerFontAtlasRectangle *allocateRectangle(unsigned int minX, unsigned int minY, unsigned int width, unsigned int height)
{
	ShovelerFontAtlasRectangle *rectangle = malloc(sizeof(ShovelerFontAtlasRectangle));
	rectangle->minX = minX;
	rectangle->minY = minY;
	rectangle->width = width;
	rectangle->height = height;
	return rectangle;
}
