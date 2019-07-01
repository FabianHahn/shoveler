#ifndef SHOVELER_FONT_H
#define SHOVELER_FONT_H

#include <stdbool.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glib.h>

typedef struct ShovelerFontsStruct {
	FT_Library library;
	/** map from string names to (ShovelerFont *) */
	GHashTable *fonts;
} ShovelerFonts;

typedef struct ShovelerFontStruct {
	ShovelerFonts *fonts;
	char *name;
	FT_Face face;
} ShovelerFont;

ShovelerFonts *shovelerFontsCreate();
ShovelerFont *shovelerFontsLoadFont(ShovelerFonts *fonts, const char *name, const char *filename);
bool shovelerFontsUnloadFont(ShovelerFonts *fonts, const char *name);
void shovelerFontsFree(ShovelerFonts *fonts);

#endif
