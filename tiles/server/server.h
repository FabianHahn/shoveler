/**
 * A tiles server:
 *  - generates a map and a world, and then seeds the world according to the map
 *  - runs a view synchronizer to manage client connections
 *  - manages player entity lifecycle for connecting clients
 */

#ifndef SHOVELER_TILES_SERVER_H
#define SHOVELER_TILES_SERVER_H

#include <shoveler/tiles/seeder.h>
#include <shoveler/view_synchronizer.h>

typedef struct ShovelerEntityIdAllocatorStruct ShovelerEntityIdAllocator;
typedef struct ShovelerMapStruct ShovelerMap;
typedef struct ShovelerSchemaStruct ShovelerSchema;
typedef struct ShovelerServerNetworkAdapterStruct ShovelerServerNetworkAdapter;
typedef struct ShovelerSystemStruct ShovelerSystem;

typedef struct ShovelerTilesServerConfigStruct {
  int chunkSize;
  int numChunkRows;
  int numChunkColumns;
  const char* tilesetPngFilename;
  int tilesetPngColumns;
  int tilesetPngRows;
  const char* character1PngFilename;
  const char* character2PngFilename;
  const char* character3PngFilename;
  const char* character4PngFilename;
  int characterShiftAmount;
} ShovelerTilesServerConfig;

typedef struct ShovelerTilesServerStruct {
  ShovelerServerNetworkAdapter* serverNetworkAdapter;
  ShovelerMap* map;
  ShovelerSystem* system;
  ShovelerSchema* schema;
  ShovelerEntityIdAllocator* entityIdAllocator;
  ShovelerViewSynchronizer* viewSynchronizer;
  ShovelerViewSynchronizerCallbacks viewSynchronizerCallbacks;
  ShovelerTilesSeeder seeder;
} ShovelerTilesServer;

ShovelerTilesServer* shovelerTilesServerCreate(
    ShovelerServerNetworkAdapter* serverNetworkAdapter, ShovelerTilesServerConfig config);
void shovelerTilesServerUpdate(ShovelerTilesServer* tilesServer);
void shovelerTilesServerFree(ShovelerTilesServer* tilesServer);

#endif
