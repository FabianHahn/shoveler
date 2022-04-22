#ifndef SHOVELER_MAP_CHUNK_H
#define SHOVELER_MAP_CHUNK_H

#include <shoveler/types.h>
#include <shoveler/map_tile.h>

typedef struct {
	unsigned char* tilesetColumns;
	unsigned char* tilesetRows;
	unsigned char* tilesetIds;
	unsigned char* tilesetColliders;
} ShovelerMapChunkTilesData;

typedef struct {
	ShovelerVector2 position;
	ShovelerMapChunkTilesData backgroundTiles;
	ShovelerMapChunkTilesData foregroundTiles;
} ShovelerMapChunk;

ShovelerMapTileData shovelerMapChunkTilesDataLookup(
	ShovelerMapChunkTilesData* chunkTilesData, int chunkSize, int tileX, int tileY);
void shovelerMapChunkTilesDataWrite(
	ShovelerMapChunkTilesData* chunkTilesData,
	int chunkSize,
	int tileX,
	int tileY,
	ShovelerMapTileData tileData);

ShovelerMapChunk* shovelerMapChunkCreate(int chunkSize);
void shovelerMapChunkInit(ShovelerMapChunk* chunk, int chunkSize);
ShovelerMapTileData shovelerMapChunkLookupTile(
	ShovelerMapChunk* chunk, int chunkSize, int tileX, int tileY, bool background);
void shovelerMapChunkWriteTile(
	ShovelerMapChunk* chunk,
	int chunkSize,
	int row,
	int column,
	bool background,
	ShovelerMapTileData tileData);
void shovelerMapChunkClear(ShovelerMapChunk* chunk);
void shovelerMapChunkFree(ShovelerMapChunk* chunk);

#endif
