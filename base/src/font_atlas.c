#include "shoveler/font_atlas.h"

#include <assert.h> // assert
#include <limits.h> // UINT_MAX INT_MAX
#include <stdlib.h> // malloc free

#include "shoveler/font.h"
#include "shoveler/image.h"
#include "shoveler/log.h"

static void insertGlyph(
    ShovelerFontAtlas* fontAtlas,
    FT_Bitmap glyph,
    int* outputPositionX,
    int* outputPositionY,
    bool* outputIsRotated);
static void growImage(ShovelerFontAtlas* fontAtlas);
static void addBitmapToImage(
    ShovelerImage* image, FT_Bitmap bitmap, int bottomLeftX, int bottomLeftY, bool isRotated);
static ShovelerFontAtlasSkylineEdge* allocateSkylineEdge(int minX, int width, int height);

ShovelerFontAtlas* shovelerFontAtlasCreate(ShovelerFont* font, int fontSize, int padding) {
  assert(fontSize >= 0);
  assert(padding >= 0);

  FT_Error error = FT_Set_Pixel_Sizes(font->face, 0, (unsigned int) fontSize);
  if (error != FT_Err_Ok) {
    shovelerLogError(
        "Failed to set pixel size to %u for font '%s': %s",
        fontSize,
        font->name,
        FT_Error_String(error));
    return NULL;
  }

  ShovelerFontAtlas* fontAtlas = malloc(sizeof(ShovelerFontAtlas));
  fontAtlas->font = font;
  fontAtlas->fontSize = fontSize;
  fontAtlas->padding = padding;
  fontAtlas->image = shovelerImageCreate(1, 1, 1);
  shovelerImageClear(fontAtlas->image);

  fontAtlas->skylineEdges = g_queue_new();
  g_queue_push_tail(fontAtlas->skylineEdges, allocateSkylineEdge(0, 1, 0));

  fontAtlas->glyphs = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, free);

  return fontAtlas;
}

ShovelerFontAtlasGlyph* shovelerFontAtlasGetGlyph(
    ShovelerFontAtlas* fontAtlas, uint32_t codePoint) {
  unsigned int glyphIndex = FT_Get_Char_Index(fontAtlas->font->face, codePoint);

  ShovelerFontAtlasGlyph* glyph = g_hash_table_lookup(fontAtlas->glyphs, &glyphIndex);
  if (glyph != NULL) {
    return glyph;
  }

  FT_Error error = FT_Load_Glyph(fontAtlas->font->face, glyphIndex, FT_LOAD_RENDER);
  if (error != FT_Err_Ok) {
    shovelerLogError(
        "Failed to load missing glyph for font '%s': %s",
        fontAtlas->font->name,
        FT_Error_String(error));
    return NULL;
  }

  unsigned int bitmapWidth = fontAtlas->font->face->glyph->bitmap.width;
  if (bitmapWidth > INT_MAX) {
    shovelerLogError(
        "Font face glyph bitmap width for code point %u out of bounds: %u",
        (unsigned int) codePoint,
        bitmapWidth);
    return NULL;
  }

  unsigned int bitmapHeight = fontAtlas->font->face->glyph->bitmap.rows;
  if (bitmapHeight > INT_MAX) {
    shovelerLogError(
        "Font face glyph bitmap rows for code point %u out of bounds: %u",
        (unsigned int) codePoint,
        bitmapHeight);
    return NULL;
  }

  long advanceX = fontAtlas->font->face->glyph->advance.x;
  if (advanceX > INT_MAX) {
    shovelerLogError(
        "Font face glyph bitmap advance X for code point %u out of bounds: %lu",
        (unsigned int) codePoint,
        advanceX);
    return NULL;
  }

  glyph = malloc(sizeof(ShovelerFontAtlasGlyph));
  glyph->index = glyphIndex;
  glyph->width = (int) bitmapWidth;
  glyph->height = (int) bitmapHeight;
  glyph->bearingX = fontAtlas->font->face->glyph->bitmap_left;
  glyph->bearingY = fontAtlas->font->face->glyph->bitmap_top;
  glyph->advance = (int) advanceX;
  insertGlyph(
      fontAtlas,
      fontAtlas->font->face->glyph->bitmap,
      &glyph->minX,
      &glyph->minY,
      &glyph->isRotated);

  g_hash_table_insert(fontAtlas->glyphs, &glyph->index, glyph);

  return glyph;
}

void shovelerFontAtlasValidateState(ShovelerFontAtlas* fontAtlas) {
  int totalWidth = 0;
  for (GList* iter = fontAtlas->skylineEdges->head; iter != NULL; iter = iter->next) {
    ShovelerFontAtlasSkylineEdge* skylineEdge = iter->data;

    assert(skylineEdge->minX < fontAtlas->image->width);
    assert(skylineEdge->width > 0);
    assert(skylineEdge->height <= fontAtlas->image->height);

    totalWidth += skylineEdge->width;
  }

  assert(totalWidth == fontAtlas->image->width);

  ShovelerImage* bitmap = shovelerImageCreate(fontAtlas->image->width, fontAtlas->image->height, 1);
  shovelerImageClear(bitmap);

  GHashTableIter iter;
  g_hash_table_iter_init(&iter, fontAtlas->glyphs);
  ShovelerFontAtlasGlyph* glyph;
  while (g_hash_table_iter_next(&iter, NULL, (gpointer*) &glyph)) {
    int placedGlyphWidth = glyph->isRotated ? glyph->height : glyph->width;
    int placedGlyphHeight = glyph->isRotated ? glyph->width : glyph->height;
    assert(glyph->minX + placedGlyphWidth <= fontAtlas->image->width);
    assert(glyph->minY + placedGlyphHeight <= fontAtlas->image->height);

    for (int x = 0; x < glyph->width; x++) {
      for (int y = 0; y < glyph->height; y++) {
        int bitmapX, bitmapY;
        if (glyph->isRotated) {
          bitmapX = glyph->minX + y;
          bitmapY = glyph->minY + x;
        } else {
          bitmapX = glyph->minX + x;
          bitmapY = glyph->minY + y;
        }

        assert(shovelerImageGet(bitmap, bitmapX, bitmapY, 0) == 0);
        shovelerImageGet(bitmap, bitmapX, bitmapY, 0) = 1;
      }
    }
  }

  shovelerImageFree(bitmap);
}

void shovelerFontAtlasFree(ShovelerFontAtlas* fontAtlas) {
  if (fontAtlas == NULL) {
    return;
  }

  g_hash_table_destroy(fontAtlas->glyphs);
  g_queue_free_full(fontAtlas->skylineEdges, free);
  shovelerImageFree(fontAtlas->image);

  free(fontAtlas);
}

static void insertGlyph(
    ShovelerFontAtlas* fontAtlas,
    FT_Bitmap glyph,
    int* outputPositionX,
    int* outputPositionY,
    bool* outputIsRotated) {
  assert(glyph.width <= INT_MAX);
  assert(glyph.rows <= INT_MAX);

  // Using the "skyline bottom left" algorithm, we try to place the glyph in a position such
  // that its height is as low as possible while also having it touch the existing ones on its
  // left. To do this, the only information we need to keep track of is the current skyline of
  // existing glyphs represented as a list of edges.
  // The skyline starts with a single edge, and every inserted glyph splits at most one edge
  // into two. This means that we have to store at most O(n) edges, but since every inserted
  // glyph can also remove any number of existing edges, there are typically fewer than n actual
  // edges in practice.
  // We find find the optimal position for the next rectangle by walking through the skyline
  // edges and doing a forward scan until we have reached enough width to place the rectangle.
  // While this is O(n^2) complexity worst case, making the overall algorithm O(n^3) worst case,
  // as mentioned before the skyline rarely actually contains n edges.
  // There exists a better algorithm to do this in O(n^2) complexity overall that also keeps
  // track of the created holes, but it is much more complicated to implement:
  // "The Bottom-Left Bin-Packing Heuristic" by Chazelle et al.

  int placedGlyphWidth = 2 * fontAtlas->padding + (int) glyph.width;
  int placedGlyphHeight = 2 * fontAtlas->padding + (int) glyph.rows;

  GList* bestStartSkylineEdgeIter = NULL;
  GList* bestEndSkylineEdgeIter = NULL;
  int bestHeight = INT_MAX;
  bool isRotated = false;
  for (GList* iter = fontAtlas->skylineEdges->head; iter != NULL; iter = iter->next) {
    ShovelerFontAtlasSkylineEdge* skylineEdge = iter->data;

    // Try to put the glyph rotated on top of this skyline edge. We try rotated first because
    // we expect glyphs to have a larger height than width.
    {
      int maxEdgeHeight = skylineEdge->height;
      // The glyph could be wider than this skyline edge, so we might have to extend across
      // a few more as long as their height is lower.
      GList* endIter = iter;
      int currentEdgeWidth = skylineEdge->width;
      while (currentEdgeWidth < placedGlyphHeight && endIter->next != NULL) {
        endIter = endIter->next;
        ShovelerFontAtlasSkylineEdge* additionalSkylineEdge = endIter->data;

        if (additionalSkylineEdge->height > maxEdgeHeight) {
          // This skyline edge is higher than the one we started with, so update our max
          // height.
          maxEdgeHeight = additionalSkylineEdge->height;
        }

        currentEdgeWidth += additionalSkylineEdge->width;
      }

      int candidateHeight = maxEdgeHeight + placedGlyphWidth;
      if (candidateHeight < bestHeight && candidateHeight <= fontAtlas->image->height &&
          currentEdgeWidth >= placedGlyphHeight) {
        // We could put the glyph here, and this is the best we've found so far.
        bestStartSkylineEdgeIter = iter;
        bestEndSkylineEdgeIter = endIter;
        bestHeight = candidateHeight;
        isRotated = true;
      }
    }

    // Try to put the glyph on top of this skyline edge.
    {
      int maxEdgeHeight = skylineEdge->height;
      // The glyph could be wider than this skyline edge, so we might have to extend across
      // a few more as long as their height is lower.
      GList* endIter = iter;
      int currentEdgeWidth = skylineEdge->width;
      while (currentEdgeWidth < placedGlyphWidth && endIter->next != NULL) {
        endIter = endIter->next;
        ShovelerFontAtlasSkylineEdge* additionalSkylineEdge = endIter->data;

        if (additionalSkylineEdge->height > maxEdgeHeight) {
          // This skyline edge is higher than the one we started with, so update our max
          // height.
          maxEdgeHeight = additionalSkylineEdge->height;
        }

        currentEdgeWidth += additionalSkylineEdge->width;
      }

      int candidateHeight = maxEdgeHeight + placedGlyphHeight;
      if (candidateHeight < bestHeight && candidateHeight <= fontAtlas->image->height &&
          currentEdgeWidth >= placedGlyphWidth) {
        // We could put the glyph here, and this is the best we've found so far.
        bestStartSkylineEdgeIter = iter;
        bestEndSkylineEdgeIter = endIter;
        bestHeight = candidateHeight;
        isRotated = false;
      }
    }
  }

  if (bestStartSkylineEdgeIter == NULL) {
    // doesn't fit, grow and retry
    growImage(fontAtlas);
    insertGlyph(fontAtlas, glyph, outputPositionX, outputPositionY, outputIsRotated);
    return;
  }

  if (isRotated) {
    int temp = placedGlyphHeight;
    placedGlyphHeight = placedGlyphWidth;
    placedGlyphWidth = temp;
  }

  ShovelerFontAtlasSkylineEdge* bestStartSkylineEdge = bestStartSkylineEdgeIter->data;
  *outputPositionX = fontAtlas->padding + bestStartSkylineEdge->minX;
  *outputPositionY = fontAtlas->padding + bestHeight - placedGlyphHeight;
  *outputIsRotated = isRotated;

  // place the glyph here
  addBitmapToImage(fontAtlas->image, glyph, *outputPositionX, *outputPositionY, *outputIsRotated);

  // Accumulate the width of all the skyline edges that we're going to combine by placing the
  // glyph on top.
  int accumulatedWidth = 0;
  for (GList* iter = bestStartSkylineEdgeIter; iter != bestEndSkylineEdgeIter;) {
    ShovelerFontAtlasSkylineEdge* currentSkylineEdge = iter->data;

    // update accumulated width
    accumulatedWidth += currentSkylineEdge->width;

    GList* currentNode = iter;
    iter = iter->next;
    free(currentSkylineEdge);
    g_queue_delete_link(fontAtlas->skylineEdges, currentNode);
  }

  // split the last edge into two
  ShovelerFontAtlasSkylineEdge* bestEndSkylineEdge = bestEndSkylineEdgeIter->data;
  accumulatedWidth += bestEndSkylineEdge->width;

  assert(accumulatedWidth >= placedGlyphWidth);

  int remainingWidth = accumulatedWidth - placedGlyphWidth;
  if (remainingWidth > 0) {
    // If we don't fill the full edge, split off the remainder
    ShovelerFontAtlasSkylineEdge* splitEndSkylineEdge = allocateSkylineEdge(
        *outputPositionX - fontAtlas->padding + placedGlyphWidth,
        remainingWidth,
        bestEndSkylineEdge->height);
    g_queue_insert_after(fontAtlas->skylineEdges, bestEndSkylineEdgeIter, splitEndSkylineEdge);
  }

  bestEndSkylineEdge->minX = *outputPositionX - fontAtlas->padding;
  bestEndSkylineEdge->width = placedGlyphWidth;
  bestEndSkylineEdge->height = bestHeight;
}

static void growImage(ShovelerFontAtlas* fontAtlas) {
  ShovelerImage* oldImage = fontAtlas->image;

  fontAtlas->image = shovelerImageCreate(2 * oldImage->width, 2 * oldImage->height, 1);
  shovelerImageClear(fontAtlas->image);
  shovelerImageAddSubImage(fontAtlas->image, 0, 0, oldImage);

  g_queue_push_tail(
      fontAtlas->skylineEdges, allocateSkylineEdge(oldImage->width, oldImage->width, 0));
  shovelerImageFree(oldImage);
}

static void addBitmapToImage(
    ShovelerImage* image, FT_Bitmap bitmap, int bottomLeftX, int bottomLeftY, bool isRotated) {
  assert(bitmap.width <= INT_MAX);
  assert(bitmap.rows <= INT_MAX);
  assert(bottomLeftX < image->width);
  assert(bottomLeftY < image->height);
  int rotatedBitmapWidth = isRotated ? (int) bitmap.rows : (int) bitmap.width;
  int rotatedBitmapHeight = isRotated ? (int) bitmap.width : (int) bitmap.rows;
  assert(
      INT_MAX - rotatedBitmapWidth >=
      image->width); // rotatedBitmapWidth + image->width won't overflow
  assert(
      INT_MAX - rotatedBitmapHeight >=
      image->height); // rotatedBitmapHeight + image->height won't overflow

  for (int bitmapX = 0; bitmapX < (int) bitmap.width; bitmapX++) {
    for (int bitmapY = 0; bitmapY < (int) bitmap.rows; bitmapY++) {
      if (isRotated) {
        shovelerImageGet(image, bottomLeftX + bitmapY, bottomLeftY + bitmapX, 0) =
            bitmap.buffer[bitmapY * bitmap.pitch + bitmapX];
      } else {
        shovelerImageGet(image, bottomLeftX + bitmapX, bottomLeftY + bitmapY, 0) =
            bitmap.buffer[(bitmap.rows - bitmapY - 1) * bitmap.pitch + bitmapX];
      }
    }
  }
}

static ShovelerFontAtlasSkylineEdge* allocateSkylineEdge(int minX, int width, int height) {
  ShovelerFontAtlasSkylineEdge* skylineEdge = malloc(sizeof(ShovelerFontAtlasSkylineEdge));
  skylineEdge->minX = minX;
  skylineEdge->width = width;
  skylineEdge->height = height;
  return skylineEdge;
}
