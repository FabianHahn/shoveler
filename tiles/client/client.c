#include "client.h"

#include <shoveler/client_network_adapter.h>
#include <shoveler/client_op.h>
#include <shoveler/client_world_updater.h>
#include <shoveler/log.h>
#include <shoveler/server_op.h>
#include <stdlib.h>

static bool receiveClientEvent(
    const ShovelerClientNetworkAdapterEvent* event, void* tilesClientPointer);

ShovelerTilesClient* shovelerTilesClientCreate(
    ShovelerClientNetworkAdapter* clientNetworkAdapter,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    ShovelerWorld* world) {
  ShovelerTilesClient* tilesClient = malloc(sizeof(ShovelerTilesClient));
  tilesClient->clientNetworkAdapter = clientNetworkAdapter;
  tilesClient->componentTypeIndexer = componentTypeIndexer;
  tilesClient->world = world;
  tilesClient->clientWorldUpdater = shovelerClientWorldUpdater(world);
  tilesClient->deserializedOp = shovelerClientOpCreateWithData(/* inputClientOp */ NULL);
  tilesClient->serializedOp = g_string_new("");

  return tilesClient;
}

void shovelerTilesClientUpdate(ShovelerTilesClient* tilesClient) {
  while (tilesClient->clientNetworkAdapter->receiveEvent(
      receiveClientEvent, tilesClient, tilesClient->clientNetworkAdapter->userData)) {
    // intentionally empty
  }
}

bool shovelerTilesClientUpdateSendAddInterest(
    ShovelerTilesClient* tilesClient, long long int entityId) {
  ShovelerServerOp serverOp;
  serverOp.type = SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST;
  serverOp.addEntityInterest.entityId = entityId;

  g_string_set_size(tilesClient->serializedOp, 0);
  if (!shovelerServerOpSerialize(
          &serverOp, tilesClient->componentTypeIndexer, tilesClient->serializedOp)) {
    shovelerLogWarning("Failed to serialize server op.");
    return false;
  }

  if (!tilesClient->clientNetworkAdapter->sendMessage(
          (const unsigned char*) tilesClient->serializedOp->str,
          (int) tilesClient->serializedOp->len,
          tilesClient->clientNetworkAdapter->userData)) {
    shovelerLogWarning("Failed to send server op.");
    return false;
  }

  return true;
}

void shovelerTilesClientFree(ShovelerTilesClient* tilesClient) {
  g_string_free(tilesClient->serializedOp, /* freeSegment */ true);
  shovelerClientOpFreeWithData(tilesClient->deserializedOp);
  free(tilesClient);
}

static bool receiveClientEvent(
    const ShovelerClientNetworkAdapterEvent* event, void* tilesClientPointer) {
  ShovelerTilesClient* tilesClient = tilesClientPointer;
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
            tilesClient->deserializedOp,
            tilesClient->componentTypeIndexer,
            (const unsigned char*) event->payload->str,
            (int) event->payload->len,
            &readIndex)) {
      shovelerLogWarning("Failed to deserialize client op.");
      return false;
    }

    ShovelerClientWorldUpdaterStatus status = shovelerClientWorldUpdaterApplyOp(
        &tilesClient->clientWorldUpdater, &tilesClient->deserializedOp->op);
    if (status != SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS &&
        status != SHOVELER_CLIENT_WORLD_UPDATER_DEPENDENCIES_INACTIVE) {
      char* clientOpDebugPrint = shovelerClientOpDebugPrint(&tilesClient->deserializedOp->op);
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
