#ifndef SHOVELER_FONT_ATLAS_H
#define SHOVELER_FONT_ATLAS_H

#include <glib.h>
#include <stdint.h> // uint32_t

typedef struct ShovelerFontStruct ShovelerFont; // forward declaration: font.h
typedef struct ShovelerImageStruct ShovelerImage; // forward declaration: image.h

typedef struct ShovelerFontAtlasSkylineEdgeStruct {
  int minX;
  int width;
  int height;
} ShovelerFontAtlasSkylineEdge;

typedef struct ShovelerFontAtlasGlyphStruct {
  unsigned int index;
  int minX;
  int minY;
  int width;
  int height;
  int bearingX;
  int bearingY;
  int advance;
  bool isRotated;
} ShovelerFontAtlasGlyph;

typedef struct ShovelerFontAtlasStruct {
  ShovelerFont* font;
  int fontSize;
  int padding;
  ShovelerImage* image;
  /** list of (ShovelerFontAtlasRectangle *) representing rectangles above the skyline */
  GQueue* skylineEdges;
  /** map from glyph index (unsigned int) to (ShovelerFontAtlasGlyph *) */
  GHashTable* glyphs;
} ShovelerFontAtlas;

ShovelerFontAtlas* shovelerFontAtlasCreate(ShovelerFont* font, int fontSize, int padding);
ShovelerFontAtlasGlyph* shovelerFontAtlasGetGlyph(ShovelerFontAtlas* fontAtlas, uint32_t codePoint);
void shovelerFontAtlasValidateState(ShovelerFontAtlas* fontAtlas);
void shovelerFontAtlasFree(ShovelerFontAtlas* fontAtlas);

#endif
