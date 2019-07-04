#ifndef SHOVELER_FONT_ATLAS_H
#define SHOVELER_FONT_ATLAS_H

#include <glib.h>

#include <shoveler/types.h>

typedef struct ShovelerFontStruct ShovelerFont; // forward declaration: font.h
typedef struct ShovelerImageStruct ShovelerImage; // forward declaration: image.h

typedef struct ShovelerFontAtlasSkylineEdgeStruct {
	unsigned int minX;
	unsigned int width;
	unsigned int height;
} ShovelerFontAtlasSkylineEdge;

typedef struct ShovelerFontAtlasStruct {
	ShovelerFont *font;
	unsigned int fontSize;
	ShovelerImage *image;
	/** list of (ShovelerFontAtlasRectangle *) representing rectangles above the skyline */
	GQueue *skylineEdges;
} ShovelerFontAtlas;

ShovelerFontAtlas *shovelerFontAtlasCreate(ShovelerFont *font, unsigned int fontSize);
void shovelerFontAtlasFree(ShovelerFontAtlas *fontAtlas);

#endif
