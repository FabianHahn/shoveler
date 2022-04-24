#include "shoveler/font_atlas_texture.h"

#include <stdlib.h> // malloc free

#include "shoveler/font_atlas.h"
#include "shoveler/image.h"
#include "shoveler/texture.h"

ShovelerFontAtlasTexture* shovelerFontAtlasTextureCreate(ShovelerFontAtlas* fontAtlas) {
  ShovelerFontAtlasTexture* fontAtlasTexture = malloc(sizeof(ShovelerFontAtlasTexture));
  fontAtlasTexture->fontAtlas = fontAtlas;
  fontAtlasTexture->atlasImageSize = 0;
  fontAtlasTexture->texture = NULL;

  return fontAtlasTexture;
}

bool shovelerFontAtlasTextureUpdate(ShovelerFontAtlasTexture* fontAtlasTexture) {
  bool newTextureGenerated = false;

  if (fontAtlasTexture->texture == NULL ||
      fontAtlasTexture->atlasImageSize != fontAtlasTexture->fontAtlas->image->width) {
    shovelerTextureFree(fontAtlasTexture->texture);
    fontAtlasTexture->atlasImageSize = fontAtlasTexture->fontAtlas->image->width;
    fontAtlasTexture->texture = shovelerTextureCreate2d(fontAtlasTexture->fontAtlas->image, false);
    newTextureGenerated = true;
  }

  // TODO: only update the texture if the font atlas is "dirty"
  shovelerTextureUpdate(fontAtlasTexture->texture);

  return newTextureGenerated;
}

void shovelerFontAtlasTextureFree(ShovelerFontAtlasTexture* fontAtlasTexture) {
  shovelerTextureFree(fontAtlasTexture->texture);
  free(fontAtlasTexture);
}
