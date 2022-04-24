#include "shoveler/map_chunk.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ShovelerMapTileData shovelerMapChunkTilesDataLookup(
    ShovelerMapChunkTilesData* chunkTilesData, int chunkSize, int tileX, int tileY) {
  assert(tileX >= 0);
  assert(tileX < chunkSize);
  assert(tileY >= 0);
  assert(tileY < chunkSize);

  ShovelerMapTileData tileData;
  tileData.tilesetColumn = chunkTilesData->tilesetColumns[tileY * chunkSize + tileX];
  tileData.tilesetRow = chunkTilesData->tilesetRows[tileY * chunkSize + tileX];
  tileData.tilesetId = chunkTilesData->tilesetIds[tileY * chunkSize + tileX];
  tileData.tilesetCollider = chunkTilesData->tilesetColliders[tileY * chunkSize + tileX];
  return tileData;
}

void shovelerMapChunkTilesDataWrite(
    ShovelerMapChunkTilesData* chunkTilesData,
    int chunkSize,
    int tileX,
    int tileY,
    ShovelerMapTileData tileData) {
  assert(tileX >= 0);
  assert(tileX < chunkSize);
  assert(tileY >= 0);
  assert(tileY < chunkSize);

  chunkTilesData->tilesetColumns[tileY * chunkSize + tileX] = tileData.tilesetColumn;
  chunkTilesData->tilesetRows[tileY * chunkSize + tileX] = tileData.tilesetRow;
  chunkTilesData->tilesetIds[tileY * chunkSize + tileX] = tileData.tilesetId;
  chunkTilesData->tilesetColliders[tileY * chunkSize + tileX] = tileData.tilesetCollider;
}

ShovelerMapChunk* shovelerMapChunkCreate(int chunkSize) {
  ShovelerMapChunk* chunk = malloc(sizeof(ShovelerMapChunk));
  shovelerMapChunkInit(chunk, chunkSize);
  return chunk;
}

void shovelerMapChunkInit(ShovelerMapChunk* chunk, int chunkSize) {
  chunk->backgroundTiles.tilesetColumns = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  chunk->backgroundTiles.tilesetRows = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  chunk->backgroundTiles.tilesetIds = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  chunk->backgroundTiles.tilesetColliders = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  chunk->foregroundTiles.tilesetColumns = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  chunk->foregroundTiles.tilesetRows = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  chunk->foregroundTiles.tilesetIds = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  chunk->foregroundTiles.tilesetColliders = malloc(chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->backgroundTiles.tilesetColumns, 0, chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->backgroundTiles.tilesetRows, 0, chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->backgroundTiles.tilesetIds, 0, chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->backgroundTiles.tilesetColliders, 0, chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->foregroundTiles.tilesetColumns, 0, chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->foregroundTiles.tilesetRows, 0, chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->foregroundTiles.tilesetIds, 0, chunkSize * chunkSize * sizeof(unsigned char));
  memset(chunk->foregroundTiles.tilesetColliders, 0, chunkSize * chunkSize * sizeof(unsigned char));
}

ShovelerMapTileData shovelerMapChunkLookupTile(
    ShovelerMapChunk* chunk, int chunkSize, int tileX, int tileY, bool background) {
  ShovelerMapChunkTilesData* tilesData;
  if (background) {
    tilesData = &chunk->backgroundTiles;
  } else {
    tilesData = &chunk->foregroundTiles;
  }

  return shovelerMapChunkTilesDataLookup(tilesData, chunkSize, tileX, tileY);
}

void shovelerMapChunkWriteTile(
    ShovelerMapChunk* chunk,
    int chunkSize,
    int tileX,
    int tileY,
    bool background,
    ShovelerMapTileData tileData) {
  ShovelerMapChunkTilesData* tilesData;
  if (background) {
    tilesData = &chunk->backgroundTiles;
  } else {
    tilesData = &chunk->foregroundTiles;
  }

  shovelerMapChunkTilesDataWrite(tilesData, chunkSize, tileX, tileY, tileData);
}

void shovelerMapChunkClear(ShovelerMapChunk* chunk) {
  free(chunk->backgroundTiles.tilesetColumns);
  free(chunk->backgroundTiles.tilesetRows);
  free(chunk->backgroundTiles.tilesetIds);
  free(chunk->backgroundTiles.tilesetColliders);
  free(chunk->foregroundTiles.tilesetColumns);
  free(chunk->foregroundTiles.tilesetRows);
  free(chunk->foregroundTiles.tilesetIds);
  free(chunk->foregroundTiles.tilesetColliders);
}

void shovelerMapChunkFree(ShovelerMapChunk* chunk) {
  if (chunk == NULL) {
    return;
  }

  shovelerMapChunkClear(chunk);
  free(chunk);
}