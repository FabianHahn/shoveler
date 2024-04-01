#include "server.h"

#include <inttypes.h>
#include <shoveler/entity_id_allocator.h>
#include <shoveler/log.h>
#include <shoveler/map.h>
#include <shoveler/schema.h>
#include <shoveler/schema/base.h>
#include <shoveler/schema/opengl.h>
#include <shoveler/server_network_adapter.h>
#include <shoveler/system.h>
#include <stdlib.h>

static void onClientConnected(
    ShovelerViewSynchronizer* viewSynchronizer, int64_t clientId, void* tilesServerPointer);
static void onClientDisconnected(
    ShovelerViewSynchronizer* viewSynchronizer,
    int64_t clientId,
    const char* reason,
    void* tilesServerPointer);

ShovelerTilesServer* shovelerTilesServerCreate(
    ShovelerServerNetworkAdapter* serverNetworkAdapter, ShovelerTilesServerConfig config) {
  ShovelerTilesServer* tilesServer = malloc(sizeof(ShovelerTilesServer));
  tilesServer->serverNetworkAdapter = serverNetworkAdapter;
  tilesServer->map =
      shovelerMapGenerate(config.chunkSize, config.numChunkRows, config.numChunkColumns);
  tilesServer->system = shovelerSystemCreate();

  tilesServer->schema = shovelerSchemaCreate();
  shovelerSchemaBaseRegister(tilesServer->schema);
  shovelerSchemaOpenglRegister(tilesServer->schema);

  tilesServer->viewSynchronizerCallbacks.onClientConnected = onClientConnected;
  tilesServer->viewSynchronizerCallbacks.onClientDisconnected = onClientDisconnected;
  tilesServer->viewSynchronizerCallbacks.userData = tilesServer;
  tilesServer->viewSynchronizer = shovelerViewSynchronizerCreate(
      tilesServer->schema,
      tilesServer->system,
      serverNetworkAdapter,
      &tilesServer->viewSynchronizerCallbacks);

  tilesServer->entityIdAllocator = shovelerEntityIdAllocatorCreate();
  tilesServer->seeder = shovelerTilesSeederInit(
      tilesServer->viewSynchronizer->world,
      tilesServer->map,
      tilesServer->entityIdAllocator,
      config.tilesetPngFilename,
      config.tilesetPngColumns,
      config.tilesetPngRows,
      config.character1PngFilename,
      config.character2PngFilename,
      config.character3PngFilename,
      config.character4PngFilename,
      config.characterShiftAmount);

  return tilesServer;
}

void shovelerTilesServerUpdate(ShovelerTilesServer* tilesServer) {
  shovelerViewSynchronizerUpdate(tilesServer->viewSynchronizer);
}

void shovelerTilesServerFree(ShovelerTilesServer* tilesServer) {
  shovelerEntityIdAllocatorFree(tilesServer->entityIdAllocator);
  shovelerViewSynchronizerFree(tilesServer->viewSynchronizer);
  shovelerSchemaFree(tilesServer->schema);
  shovelerSystemFree(tilesServer->system);
  shovelerMapFree(tilesServer->map);
  free(tilesServer);
}

static void onClientConnected(
    ShovelerViewSynchronizer* viewSynchronizer, int64_t clientId, void* tilesServerPointer) {
  ShovelerTilesServer* tilesServer = tilesServerPointer;
  shovelerLogInfo("Server connected client %" PRIu64 ".", clientId);
  shovelerTilesSeederSpawnPlayer(&tilesServer->seeder, shovelerVector2(0.5f, 0.5f));
}

static void onClientDisconnected(
    ShovelerViewSynchronizer* viewSynchronizer,
    int64_t clientId,
    const char* reason,
    void* tilesServerPointer) {
  ShovelerTilesServer* tilesServer = tilesServerPointer;
  shovelerLogInfo("Server disconnected client %" PRIu64 ": %s.", clientId, reason);
}
