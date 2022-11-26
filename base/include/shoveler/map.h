#ifndef SHOVELER_MAP_H
#define SHOVELER_MAP_H

#include <glib.h>
#include <shoveler/map_chunk.h>
#include <shoveler/map_dimensions.h>
#include <shoveler/map_tile.h>
#include <shoveler/types.h>

typedef struct ShovelerMapStruct {
  ShovelerMapDimensions dimensions;
  ShovelerMapChunk* chunks;
} ShovelerMap;

bool shovelerMapEntityHandleToChunkCoordinates(
    int64_t entityHandle,
    int64_t firstChunkEntityHandle,
    ShovelerMapDimensions dimensions,
    int* outputChunkX,
    int* outputChunkY);
ShovelerMap* shovelerMapGenerate(int chunkSize, int numChunkRows, int numChunkColumns);
bool shovelerMapCheckTileCollision(
    ShovelerMap* map, ShovelerVector2 position, ShovelerVector2 size);
bool shovelerMapCheckPointCollision(ShovelerMap* map, ShovelerVector2 position);
ShovelerMapTileData shovelerMapLookupTile(
    ShovelerMap* map, ShovelerMapTileCoordinate tile, bool background);
void shovelerMapWriteTile(
    ShovelerMap* map,
    ShovelerMapTileCoordinate tile,
    bool background,
    ShovelerMapTileData tileData);
ShovelerMapChunk* shovelerMapGetChunk(ShovelerMap* map, int chunkX, int chunkY);
void shovelerMapFree(ShovelerMap* map);

#endif
