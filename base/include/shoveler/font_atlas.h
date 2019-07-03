#ifndef SHOVELER_FONT_ATLAS_H
#define SHOVELER_FONT_ATLAS_H

#include <stdint.h> // uint32_t

#include <glib.h>

typedef struct ShovelerFontStruct ShovelerFont; // forward declaration: font.h
typedef struct ShovelerImageStruct ShovelerImage; // forward declaration: image.h

typedef struct ShovelerFontAtlasSkylineEdgeStruct {
	unsigned int minX;
	unsigned int width;
	unsigned int height;
} ShovelerFontAtlasSkylineEdge;

typedef struct ShovelerFontAtlasGlyphStruct {
	unsigned int index;
	unsigned int minX;
	unsigned int minY;
	unsigned int width;
	unsigned int height;
	bool isRotated;
} ShovelerFontAtlasGlyph;

typedef struct ShovelerFontAtlasStruct {
	ShovelerFont *font;
	unsigned int fontSize;
	ShovelerImage *image;
	/** list of (ShovelerFontAtlasRectangle *) representing rectangles above the skyline */
	GQueue *skylineEdges;
	/** map from glyph index (unsigned int) to (ShovelerFontAtlasGlyph *) */
	GHashTable *glyphs;
} ShovelerFontAtlas;

ShovelerFontAtlas *shovelerFontAtlasCreate(ShovelerFont *font, unsigned int fontSize);
ShovelerFontAtlasGlyph *shovelerFontAtlasGetGlyph(ShovelerFontAtlas *fontAtlas, uint32_t codePoint);
void shovelerFontAtlasValidateState(ShovelerFontAtlas *fontAtlas);
void shovelerFontAtlasFree(ShovelerFontAtlas *fontAtlas);

#endif
