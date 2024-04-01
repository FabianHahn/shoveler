#ifndef SHOVELER_TILES_SEEDER_H
#define SHOVELER_TILES_SEEDER_H

#include <shoveler/types.h>

typedef struct ShovelerEntityIdAllocatorStruct ShovelerEntityIdAllocator;
typedef struct ShovelerMapStruct ShovelerMap;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef struct ShovelerTilesSeederStruct {
  ShovelerWorld* world;
  ShovelerMap* map;
  ShovelerEntityIdAllocator* entityIdAllocator;
  long long int quadDrawableEntityId;
  long long int tilesetEntityId;
  long long int tilesetPngEntityId;
  long long int characterTilesetEntityIds[4];
  long long int canvasEntityId;
  long long int tilemapMaterialEntityId;
  int nextPlayerTilesetIndex;
} ShovelerTilesSeeder;

ShovelerTilesSeeder shovelerTilesSeederInit(
    ShovelerWorld* world,
    ShovelerMap* map,
    ShovelerEntityIdAllocator* entityIdAllocator,
    const char* tilesetPngFilename,
    int tilesetPngColumns,
    int tilesetPngRows,
    const char* character1PngFilename,
    const char* character2PngFilename,
    const char* character3PngFilename,
    const char* character4PngFilename,
    int characterShiftAmount);

void shovelerTilesSeederSpawnPlayer(ShovelerTilesSeeder* seeder, ShovelerVector2 position);

#endif
