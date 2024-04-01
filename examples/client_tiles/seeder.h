#ifndef SHOVELER_EXAMPLES_CLIENT_TILES_SEEDER_H
#define SHOVELER_EXAMPLES_CLIENT_TILES_SEEDER_H

#include <shoveler/types.h>

typedef struct ShovelerMapStruct ShovelerMap;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef struct ShovelerClientTilesSeeder {
  ShovelerWorld* world;
  ShovelerMap* map;
  long long int* nextEntityId;
  long long int quadDrawableEntityId;
  long long int tilesetEntityId;
  long long int characterTilesetEntityIds[4];
  long long int canvasEntityId;
  long long int tilemapMaterialEntityId;
  int nextPlayerTilesetIndex;
} ShovelerClientTilesSeeder;

ShovelerClientTilesSeeder shovelerClientTilesSeederInit(
    ShovelerWorld* world,
    ShovelerMap* map,
    long long int* nextEntityId,
    const char* tilesetPngFilename,
    int tilesetPngColumns,
    int tilesetPngRows,
    const char* character1PngFilename,
    const char* character2PngFilename,
    const char* character3PngFilename,
    const char* character4PngFilename,
    int characterShiftAmount);

void shovelerClientTilesSeederSpawnPlayer(
    ShovelerClientTilesSeeder* context, ShovelerVector2 position);

#endif
