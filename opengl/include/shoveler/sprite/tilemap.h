#ifndef SHOVELER_SPRITE_TILEMAP_H
#define SHOVELER_SPRITE_TILEMAP_H

#include <shoveler/sprite.h>

typedef struct ShovelerTilemapStruct ShovelerTilemap; // forward declaration: tilemap.h

typedef struct ShovelerSpriteTilemapStruct {
  ShovelerSprite sprite;
  ShovelerTilemap* tilemap;
} ShovelerSpriteTilemap;

// Create a tilemap sprite from a tilemap material and a tilemap, with the caller retaining
// ownership over both.
ShovelerSprite* shovelerSpriteTilemapCreate(ShovelerMaterial* material, ShovelerTilemap* tilemap);

#endif
