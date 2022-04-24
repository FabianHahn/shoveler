#ifndef SHOVELER_FONT_H
#define SHOVELER_FONT_H

#include <ft2build.h>
#include <stdbool.h>
#include FT_FREETYPE_H

#include <glib.h>

typedef struct ShovelerFontsStruct {
  FT_Library library;
  /** map from string names to (ShovelerFont *) */
  GHashTable* fonts;
} ShovelerFonts;

typedef struct ShovelerFontStruct {
  ShovelerFonts* fonts;
  char* name;
  FT_Face face;
} ShovelerFont;

ShovelerFonts* shovelerFontsCreate();
ShovelerFont* shovelerFontsLoadFontFile(
    ShovelerFonts* fonts, const char* name, const char* filename);
ShovelerFont* shovelerFontsLoadFontBuffer(
    ShovelerFonts* fonts, const char* name, const unsigned char* buffer, int bufferSize);
bool shovelerFontsUnloadFont(ShovelerFonts* fonts, const char* name);
void shovelerFontsFree(ShovelerFonts* fonts);

#endif
