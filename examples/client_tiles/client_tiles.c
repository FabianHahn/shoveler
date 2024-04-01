#include <shoveler/camera/perspective.h>
#include <shoveler/client_op.h>
#include <shoveler/client_property_manager.h>
#include <shoveler/client_system.h>
#include <shoveler/client_world_updater.h>
#include <shoveler/component.h>
#include <shoveler/component_field.h>
#include <shoveler/component_type.h>
#include <shoveler/constants.h>
#include <shoveler/file.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/image/png.h>
#include <shoveler/in_memory_network_adapter.h>
#include <shoveler/map.h>
#include <shoveler/material/texture.h>
#include <shoveler/opengl.h>
#include <shoveler/server_op.h>
#include <shoveler/system.h>
#include <shoveler/types.h>
#include <shoveler/view_synchronizer.h>
#include <shoveler/world.h>
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include "seeder.h"

static double time = 0.0;

static void updateGame(ShovelerGame* game, double dt);
static bool receiveClientEvent(
    const ShovelerClientNetworkAdapterEvent* event, void* clientContextPointer);
static void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* userData);
static void onClientConnected(
    ShovelerViewSynchronizer* viewSynchronizer, int64_t clientId, void* serverContextPointer);
static void onClientDisconnected(
    ShovelerViewSynchronizer* viewSynchronizer,
    int64_t clientId,
    const char* reason,
    void* serverContextPointer);

typedef struct {
  ShovelerViewSynchronizer* viewSynchronizer;
  long long int nextEntityId;
  ShovelerClientTilesSeeder seeder;
} ShovelerServerContext;

typedef struct {
  void* clientHandle;
  ShovelerClientNetworkAdapter* client;
  ShovelerWorld* world;
  ShovelerClientWorldUpdater clientWorldUpdater;
  ShovelerComponentTypeIndexer* componentTypeIndexer;
  ShovelerClientOpWithData* deserializedOp;
} ShovelerClientContext;

int main(int argc, char* argv[]) {
  shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
  shovelerGlobalInit();

  if (argc != 9) {
    shovelerLogError(
        "Usage:\n\t%s <tileset png> <tileset columns> <tileset rows> <character png> <character2 "
        "png> <character3 png> <character4 png> <character shift amount>",
        argv[0]);
    return EXIT_FAILURE;
  }

  const char* tilesetPngFilename = argv[1];
  int tilesetPngColumns = atoi(argv[2]);
  int tilesetPngRows = atoi(argv[3]);
  const char* character1PngFilename = argv[4];
  const char* character2PngFilename = argv[5];
  const char* character3PngFilename = argv[6];
  const char* character4PngFilename = argv[7];
  int characterShiftAmount = atoi(argv[8]);

  ShovelerGameWindowSettings windowSettings;
  windowSettings.windowTitle = "shoveler";
  windowSettings.fullscreen = false;
  windowSettings.vsync = true;
  windowSettings.samples = 4;
  windowSettings.windowedWidth = 640;
  windowSettings.windowedHeight = 480;

  ShovelerGameCameraSettings cameraSettings;
  cameraSettings.frame.position = shovelerVector3(0, 0, 5);
  cameraSettings.frame.direction = shovelerVector3(0, 0, -1);
  cameraSettings.frame.up = shovelerVector3(0, 1, 0);
  cameraSettings.projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
  cameraSettings.projection.aspectRatio =
      (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
  cameraSettings.projection.nearClippingPlane = 0.01;
  cameraSettings.projection.farClippingPlane = 1000;

  ShovelerGameControllerSettings controllerSettings;
  controllerSettings.frame = cameraSettings.frame;
  controllerSettings.moveFactor = 2.0f;
  controllerSettings.tiltFactor = 0.0005f;
  controllerSettings.boundingBoxSize2 = 0.9f;
  controllerSettings.boundingBoxSize3 = 0.0f;

  shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
  shovelerGlobalInit();

  ShovelerGame* game =
      shovelerGameCreate(updateGame, &windowSettings, &cameraSettings, &controllerSettings);
  if (game == NULL) {
    return EXIT_FAILURE;
  }

  game->controller->lockTiltX = true;
  game->controller->lockTiltY = true;

  int chunkSize = 10;
  int numChunkRows = 10;
  int numChunkColumns = 10;
  ShovelerMap* map = shovelerMapGenerate(chunkSize, numChunkRows, numChunkColumns);

  ShovelerWorldCallbacks worldCallbacks = shovelerWorldCallbacks();
  worldCallbacks.onUpdateComponent = onUpdateComponent;
  ShovelerClientSystem* clientSystem = shovelerClientSystemCreate(game, &worldCallbacks);

  ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter = shovelerInMemoryNetworkAdapterCreate();
  ShovelerServerNetworkAdapter* serverNetworkAdapter =
      shovelerInMemoryNetworkAdapterGetServer(inMemoryNetworkAdapter);

  ShovelerServerContext serverContext;
  serverContext.nextEntityId = 1;

  ShovelerViewSynchronizerCallbacks viewSynchronizerCallbacks;
  viewSynchronizerCallbacks.onClientConnected = onClientConnected;
  viewSynchronizerCallbacks.onClientDisconnected = onClientDisconnected;
  viewSynchronizerCallbacks.userData = &serverContext;
  ShovelerSystem* serverSystem = shovelerSystemCreate();
  serverContext.viewSynchronizer = shovelerViewSynchronizerCreate(
      clientSystem->schema, serverSystem, serverNetworkAdapter, &viewSynchronizerCallbacks);

  ShovelerClientContext clientContext;
  clientContext.clientHandle = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  clientContext.client =
      shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientContext.clientHandle);
  clientContext.world = clientSystem->world;
  clientContext.clientWorldUpdater = shovelerClientWorldUpdater(clientContext.world);
  clientContext.componentTypeIndexer = serverContext.viewSynchronizer->componentTypeIndexer;
  clientContext.deserializedOp = shovelerClientOpCreateWithData(/* inputClientOp */ NULL);

  serverContext.seeder = shovelerClientTilesSeederInit(
      serverContext.viewSynchronizer->world,
      map,
      &serverContext.nextEntityId,
      tilesetPngFilename,
      tilesetPngColumns,
      tilesetPngRows,
      character1PngFilename,
      character2PngFilename,
      character3PngFilename,
      character4PngFilename,
      characterShiftAmount);

  shovelerClientTilesSeederSpawnPlayer(&serverContext.seeder, shovelerVector2(0.5f, 0.5f));
  
  GString* serializedOp = g_string_new("");
  for (long long int entityId = 1; entityId < serverContext.nextEntityId; entityId++) {
    ShovelerServerOp serverOp;
    serverOp.type = SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST;
    serverOp.addEntityInterest.entityId = entityId;

    g_string_set_size(serializedOp, 0);
    if (!shovelerServerOpSerialize(&serverOp, clientContext.componentTypeIndexer, serializedOp)) {
      shovelerLogWarning("Failed to serialize server op.");
      break;
    }

    if (!clientContext.client->sendMessage(
            (const unsigned char*) serializedOp->str,
            (int) serializedOp->len,
            clientContext.client->userData)) {
      shovelerLogWarning("Failed to send server op.");
      break;
    }
  }
  g_string_free(serializedOp, /* freeSegment */ true);

  shovelerOpenGLCheckSuccess();

  while (shovelerGameIsRunning(game)) {
    shovelerViewSynchronizerUpdate(serverContext.viewSynchronizer);

    while (clientContext.client->receiveEvent(
        receiveClientEvent, &clientContext, clientContext.client->userData)) {
    }

    shovelerGameRenderFrame(game);
  }
  shovelerLogInfo("Exiting main loop, goodbye.");

  shovelerClientOpFreeWithData(clientContext.deserializedOp);

  shovelerInMemoryNetworkAdapterDisconnectClient(
      inMemoryNetworkAdapter, clientContext.clientHandle, "shutting down");
  shovelerViewSynchronizerUpdate(serverContext.viewSynchronizer);

  shovelerViewSynchronizerFree(serverContext.viewSynchronizer);
  shovelerSystemFree(serverSystem);
  shovelerInMemoryNetworkAdapterFree(inMemoryNetworkAdapter);
  shovelerClientSystemFree(clientSystem);
  shovelerMapFree(map);
  shovelerGameFree(game);
  shovelerGlobalUninit();
  shovelerLogTerminate();

  return EXIT_SUCCESS;
}

static void updateGame(ShovelerGame* game, double dt) {
  shovelerCameraUpdateView(game->camera);

  time += dt;
}

static bool receiveClientEvent(
    const ShovelerClientNetworkAdapterEvent* event, void* clientContextPointer) {
  ShovelerClientContext* clientContext = clientContextPointer;
  switch (event->type) {
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_NONE:
    break;
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED:
    shovelerLogInfo("Client connected to server.");
    break;
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED:
    shovelerLogInfo("Client disconnected from server: %s", event->payload->str);
    break;
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE: {
    int readIndex = 0;
    if (!shovelerClientOpDeserialize(
            clientContext->deserializedOp,
            clientContext->componentTypeIndexer,
            (const unsigned char*) event->payload->str,
            (int) event->payload->len,
            &readIndex)) {
      shovelerLogWarning("Failed to deserialize client op.");
      return false;
    }

    ShovelerClientWorldUpdaterStatus status = shovelerClientWorldUpdaterApplyOp(
        &clientContext->clientWorldUpdater, &clientContext->deserializedOp->op);
    if (status != SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS &&
        status != SHOVELER_CLIENT_WORLD_UPDATER_DEPENDENCIES_INACTIVE) {
      char* clientOpDebugPrint = shovelerClientOpDebugPrint(&clientContext->deserializedOp->op);
      shovelerLogWarning(
          "Failed to apply client op %s: %s",
          clientOpDebugPrint,
          shovelerClientWorldUpdaterStatusToString(status));
      free(clientOpDebugPrint);
      return false;
    }
  } break;
  }

  return true;
}

static void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* userData) {
  if (!isAuthoritative) {
    return;
  }

  GString* printedValue = shovelerComponentFieldPrintValue(value);

  shovelerLogTrace(
      "Updating field '%s' of component '%s' on entity %lld: %s",
      field->name,
      component->type->id,
      component->entityId,
      printedValue->str);

  g_string_free(printedValue, true);
}

static void onClientConnected(
    ShovelerViewSynchronizer* viewSynchronizer, int64_t clientId, void* serverContextPointer) {
  ShovelerServerContext* serverContext = serverContextPointer;
  shovelerLogInfo("Server connected client %" PRIu64 ".", clientId);
  // TODO: make sure client components get activated
}

static void onClientDisconnected(
    ShovelerViewSynchronizer* viewSynchronizer,
    int64_t clientId,
    const char* reason,
    void* serverContextPointer) {
  shovelerLogInfo("Server disconnected client %" PRIu64 ": %s.", clientId, reason);
}
