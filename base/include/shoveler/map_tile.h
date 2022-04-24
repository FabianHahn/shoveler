#ifndef SHOVELER_MAP_TILE_H
#define SHOVELER_MAP_TILE_H

#include <stdbool.h>

typedef struct {
  unsigned char tilesetColumn;
  unsigned char tilesetRow;
  unsigned char tilesetId;
  unsigned char tilesetCollider;
} ShovelerMapTileData;

static inline ShovelerMapTileData shovelerMapTileData() {
  ShovelerMapTileData tileData;
  tileData.tilesetColumn = 0;
  tileData.tilesetRow = 0;
  tileData.tilesetId = 0;
  tileData.tilesetCollider = false;
  return tileData;
}

#endif
