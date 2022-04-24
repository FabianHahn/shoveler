#include "shoveler/font.h"

#include <stdlib.h> // malloc free
#include <string.h> // strdup

#include "shoveler/log.h"

static void freeFont(void* fontPointer);

ShovelerFonts* shovelerFontsCreate() {
  FT_Library library;
  FT_Error error = FT_Init_FreeType(&library);
  if (error != FT_Err_Ok) {
    shovelerLogError("Failed to initialize freetype library: %s", FT_Error_String(error));
    return NULL;
  }

  ShovelerFonts* fonts = malloc(sizeof(ShovelerFonts));
  fonts->library = library;
  fonts->fonts = g_hash_table_new_full(&g_str_hash, &g_str_equal, NULL, freeFont);

  return fonts;
}

ShovelerFont* shovelerFontsLoadFontFile(
    ShovelerFonts* fonts, const char* name, const char* filename) {
  ShovelerFont* font = g_hash_table_lookup(fonts->fonts, name);
  if (font != NULL) {
    return font;
  }

  FT_Face face;
  FT_Error error = FT_New_Face(fonts->library, filename, 0, &face);
  if (error != FT_Err_Ok) {
    shovelerLogError(
        "Failed to load font '%s' from '%s': %s", name, filename, FT_Error_String(error));
    return NULL;
  }

  font = malloc(sizeof(ShovelerFont));
  font->fonts = fonts;
  font->name = strdup(name);
  font->face = face;
  g_hash_table_insert(fonts->fonts, font->name, font);

  return font;
}

ShovelerFont* shovelerFontsLoadFontBuffer(
    ShovelerFonts* fonts, const char* name, const unsigned char* buffer, int bufferSize) {
  ShovelerFont* font = g_hash_table_lookup(fonts->fonts, name);
  if (font != NULL) {
    return font;
  }

  FT_Face face;
  FT_Error error = FT_New_Memory_Face(fonts->library, buffer, bufferSize, 0, &face);
  if (error != FT_Err_Ok) {
    shovelerLogError(
        "Failed to load font '%s' from buffer of size %d: %s",
        name,
        bufferSize,
        FT_Error_String(error));
    return NULL;
  }

  font = malloc(sizeof(ShovelerFont));
  font->fonts = fonts;
  font->name = strdup(name);
  font->face = face;
  g_hash_table_insert(fonts->fonts, font->name, font);

  return font;
}

bool shovelerFontsUnloadFont(ShovelerFonts* font, const char* name) {
  return g_hash_table_remove(font->fonts, name);
}

void shovelerFontsFree(ShovelerFonts* fonts) {
  if (fonts == NULL) {
    return;
  }

  g_hash_table_destroy(fonts->fonts);
  FT_Done_FreeType(fonts->library);

  free(fonts);
}

static void freeFont(void* fontPointer) {
  ShovelerFont* font = (ShovelerFont*) fontPointer;

  free(font->name);
  FT_Done_Face(font->face);

  free(font);
}
