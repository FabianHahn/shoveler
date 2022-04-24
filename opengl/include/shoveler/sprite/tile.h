#ifndef SHOVELER_SPRITE_TILE_H
#define SHOVELER_SPRITE_TILE_H

#include <shoveler/sprite.h>

typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h

typedef struct ShovelerSpriteTileStruct {
  ShovelerSprite sprite;
  ShovelerTileset* tileset;
  int tilesetRow;
  int tilesetColumn;
} ShovelerSpriteTile;

ShovelerSprite* shovelerSpriteTileCreate(
    ShovelerMaterial* material, ShovelerTileset* tileset, int tilesetRow, int tilesetColumn);
ShovelerTileset* shovelerSpriteTileGetTileset(ShovelerSprite* sprite);
void shovelerSpriteTileSetIndices(ShovelerSprite* sprite, int tilesetRow, int tilesetColumn);

#endif
