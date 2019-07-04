#include <assert.h> // assert
#include <limits.h> // UINT_MAX
#include <stdlib.h> // malloc free

#include "shoveler/font.h"
#include "shoveler/font_atlas.h"
#include "shoveler/log.h"
#include "shoveler/image.h"

static void insertGlyph(ShovelerFontAtlas *fontAtlas, FT_Bitmap bitmap);
static void growImage(ShovelerFontAtlas *fontAtlas);
static void addBitmapToImage(ShovelerImage *image, FT_Bitmap bitmap, unsigned bottomLeftX, unsigned bottomLeftY, bool isRotated);
static ShovelerFontAtlasSkylineEdge *allocateSkylineEdge(unsigned int minX, unsigned int width, unsigned int height);

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
	g_queue_push_tail(fontAtlas->skylineEdges, allocateSkylineEdge(0, 64, 0));

	insertGlyph(fontAtlas, font->face->glyph->bitmap);

	for(char c = 'a'; c <= 'z'; c++) {
		FT_Load_Char(font->face, c, FT_LOAD_RENDER);
		insertGlyph(fontAtlas, font->face->glyph->bitmap);
	}

	for(char c = 'A'; c <= 'Z'; c++) {
		FT_Load_Char(font->face, c, FT_LOAD_RENDER);
		insertGlyph(fontAtlas, font->face->glyph->bitmap);
	}

	for(char c = '0'; c <= '9'; c++) {
		FT_Load_Char(font->face, c, FT_LOAD_RENDER);
		insertGlyph(fontAtlas, font->face->glyph->bitmap);
	}

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
		ShovelerFontAtlasSkylineEdge *skylineEdge = iter->data;

		// try to put the bitmap on top of this skyline edge
		unsigned int candidateHeight = skylineEdge->height + bitmap.rows;
		if(candidateHeight < bestHeight && candidateHeight <= fontAtlas->image->height) {
			// the bitmap could be wider than this skyline edge, so we might have to extend across a few more as long as their height is lower
			GList *endIter = iter;
			unsigned int currentEdgeWidth = skylineEdge->width;
			while(currentEdgeWidth < bitmap.width && endIter->next != NULL) {
				endIter = endIter->next;
				ShovelerFontAtlasSkylineEdge *additionalSkylineEdge = endIter->data;

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

		// try to put the bitmap rotated on top of this skyline edge
		candidateHeight = skylineEdge->height + bitmap.width;
		if(candidateHeight < bestHeight && candidateHeight <= fontAtlas->image->height) {
			// the bitmap could be wider than this skyline edge, so we might have to extend across a few more as long as their height is lower
			GList *endIter = iter;
			unsigned int currentEdgeWidth = skylineEdge->width;
			while(currentEdgeWidth < bitmap.rows && endIter->next != NULL) {
				endIter = endIter->next;
				ShovelerFontAtlasSkylineEdge *additionalSkylineEdge = endIter->data;

				if(additionalSkylineEdge->height > skylineEdge->height) {
					// this skyline edge is higher than the one we started with, but the bitmap doesn't fit yet - abort
					break;
				}

				currentEdgeWidth += additionalSkylineEdge->width;
			}

			if(currentEdgeWidth >= bitmap.rows) {
				// we could put the bitmap here, and this is the best we've found so far
				bestStartSkylineEdgeIter = iter;
				bestEndSkylineEdgeIter = endIter;
				bestHeight = candidateHeight;
				isRotated = true;
			}
		}
	}

	if(bestStartSkylineEdgeIter == NULL) {
		// doesn't fit, grow and retry
		growImage(fontAtlas);
		insertGlyph(fontAtlas, bitmap);
		return;
	}

	ShovelerFontAtlasSkylineEdge *bestStartSkylineEdge = bestStartSkylineEdgeIter->data;

	// place the bitmap here
	addBitmapToImage(fontAtlas->image, bitmap, bestStartSkylineEdge->minX, bestStartSkylineEdge->height, isRotated);
	
	GList *currentSkylineEdgeIter = bestStartSkylineEdgeIter;
	unsigned int remainingWidth = isRotated ? bitmap.rows : bitmap.width;
	for(GList *iter = bestStartSkylineEdgeIter; iter != bestEndSkylineEdgeIter; iter = iter->next) {
		ShovelerFontAtlasSkylineEdge *currentSkylineEdge = iter->data;

		// lift it up to the new height
		currentSkylineEdge->height = bestHeight;

		// update remaining width
		remainingWidth -= currentSkylineEdge->width;
	}

	// split the last edge into two
	ShovelerFontAtlasSkylineEdge *bestEndSkylineEdge = bestEndSkylineEdgeIter->data;
	
	if(remainingWidth < bestEndSkylineEdge->width) {
		// If we don't fill the full edge, split off the remainder
		ShovelerFontAtlasSkylineEdge* splitEndSkylineEdge = allocateSkylineEdge(bestEndSkylineEdge->minX + remainingWidth, bestEndSkylineEdge->width - remainingWidth, bestEndSkylineEdge->height);
		g_queue_insert_after(fontAtlas->skylineEdges, bestEndSkylineEdgeIter, splitEndSkylineEdge);
	}

	bestEndSkylineEdge->width = remainingWidth;
	bestEndSkylineEdge->height = bestHeight;
}

static void growImage(ShovelerFontAtlas *fontAtlas)
{
	ShovelerImage *oldImage = fontAtlas->image;

	fontAtlas->image = shovelerImageCreate(2 * oldImage->width, 2 * oldImage->height, 1);
	shovelerImageClear(fontAtlas->image);
	shovelerImageAddSubImage(fontAtlas->image, 0, 0, oldImage);

	g_queue_push_tail(fontAtlas->skylineEdges, allocateSkylineEdge(oldImage->width, oldImage->width, 0));
}

static void addBitmapToImage(ShovelerImage *image, FT_Bitmap bitmap, unsigned bottomLeftX, unsigned bottomLeftY, bool isRotated)
{
	assert(bottomLeftX < image->width);
	assert(bottomLeftY < image->height);
	unsigned int rotatedBitmapWidth = isRotated ? bitmap.rows : bitmap.width;
	unsigned int rotatedBitmapHeight = isRotated ? bitmap.width : bitmap.rows;
	assert(UINT_MAX - rotatedBitmapWidth >= image->width); // rotatedBitmapWidth + image->width won't overflow
	assert(UINT_MAX - rotatedBitmapHeight >= image->height); // rotatedBitmapHeight + image->height won't overflow

	for(unsigned int bitmapX = 0; bitmapX < bitmap.width; bitmapX++) {
		for(unsigned int bitmapY = 0; bitmapY < bitmap.rows; bitmapY++) {
			if(isRotated) {
				shovelerImageGet(image, bottomLeftX + bitmapY, bottomLeftY + bitmapX, 0) = bitmap.buffer[bitmapY * bitmap.pitch + bitmapX];
			} else {
				shovelerImageGet(image, bottomLeftX + bitmapX, bottomLeftY + bitmapY, 0) = bitmap.buffer[(bitmap.rows - bitmapY - 1) * bitmap.pitch + bitmapX];
			}
		}
	}
}

static ShovelerFontAtlasSkylineEdge *allocateSkylineEdge(unsigned int minX, unsigned int width, unsigned int height)
{
	ShovelerFontAtlasSkylineEdge *skylineEdge = malloc(sizeof(ShovelerFontAtlasSkylineEdge));
	skylineEdge->minX = minX;
	skylineEdge->width = width;
	skylineEdge->height = height;
	return skylineEdge;
}
