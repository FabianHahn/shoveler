#ifndef SHOVELER_FONT_ATLAS_H
#define SHOVELER_FONT_ATLAS_H

typedef struct ShovelerFontStruct ShovelerFont; // forward declaration: font.h
typedef struct ShovelerImageStruct ShovelerImage; // forward declaration: image.h

typedef struct ShovelerFontAtlasStruct {
	ShovelerFont *font;
	unsigned int fontSize;
	ShovelerImage *image;
} ShovelerFontAtlas;

ShovelerFontAtlas *shovelerFontAtlasCreate(ShovelerFont *font, unsigned int fontSize);
void shovelerFontAtlasFree(ShovelerFontAtlas *fontAtlas);

#endif
