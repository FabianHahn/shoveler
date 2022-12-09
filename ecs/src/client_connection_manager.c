#include "shoveler/client_connection_manager.h"

#include <inttypes.h>
#include <shoveler/client_op.h>
#include <shoveler/log.h>
#include <shoveler/server_network_adapter.h>
#include <shoveler/server_op.h>
#include <stdlib.h>

bool receiveEvent(const ShovelerServerNetworkAdapterEvent* event, void* userData);
static void freeClientConnection(void* clientConnectionPointer);

ShovelerClientConnectionManager* shovelerClientConnectionManagerCreate(
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    ShovelerServerNetworkAdapter* networkAdapter,
    ShovelerClientConnectionManagerCallbacks* callbacks) {
  ShovelerClientConnectionManager* clientConnectionManager =
      malloc(sizeof(ShovelerClientConnectionManager));
  clientConnectionManager->componentTypeIndexer = componentTypeIndexer;
  clientConnectionManager->networkAdapter = networkAdapter;
  clientConnectionManager->callbacks = callbacks;
  clientConnectionManager->clients = g_hash_table_new_full(
      g_int64_hash, g_int64_equal, /* keyDestroyFunc */ NULL, freeClientConnection);
  clientConnectionManager->clientsByHandle = g_hash_table_new(g_direct_hash, g_direct_equal);
  clientConnectionManager->nextClientId = 0;
  clientConnectionManager->buffer = g_string_new("");
  clientConnectionManager->serverOp = shovelerServerOpCreateWithData(/* input */ NULL);
  return clientConnectionManager;
}

void shovelerClientConnectionManagerFree(ShovelerClientConnectionManager* clientConnectionManager) {
  shovelerServerOpFreeWithData(clientConnectionManager->serverOp);
  g_string_free(clientConnectionManager->buffer, /* freeSegment */ true);
  g_hash_table_destroy(clientConnectionManager->clientsByHandle);
  g_hash_table_destroy(clientConnectionManager->clients);
  free(clientConnectionManager);
}

int shovelerClientConnectionManagerUpdate(
    ShovelerClientConnectionManager* clientConnectionManager) {
  int numEvents = 0;
  while (clientConnectionManager->networkAdapter->receiveEvent(
      receiveEvent, clientConnectionManager, clientConnectionManager->networkAdapter->userData)) {
    numEvents++;
  }
  return numEvents;
}

int shovelerClientConnectionManagerSendClientOp(
    ShovelerClientConnectionManager* clientConnectionManager,
    const int64_t* clientIds,
    int numClients,
    const ShovelerClientOp* clientOp) {
  g_string_set_size(clientConnectionManager->buffer, 0);
  if (!shovelerClientOpSerialize(
          clientOp,
          clientConnectionManager->componentTypeIndexer,
          clientConnectionManager->buffer)) {
    return 0;
  }

  int numSent = 0;
  for (int i = 0; i < numClients; i++) {
    ShovelerClientConnection* clientConnection =
        g_hash_table_lookup(clientConnectionManager->clients, &clientIds[i]);
    if (clientConnection == NULL) {
      continue;
    }

    if (!clientConnectionManager->networkAdapter->sendMessage(
            clientConnection->handle,
            (const unsigned char*) clientConnectionManager->buffer->str,
            (int) clientConnectionManager->buffer->len,
            clientConnectionManager->networkAdapter->userData)) {
      char* debugPrint = shovelerClientOpDebugPrint(clientOp);
      shovelerLogWarning(
          "Failed to send op to client %" PRId64 " (%p): %s",
          clientIds[i],
          clientConnection->handle,
          debugPrint);
      free(debugPrint);
      break;
    }
    numSent++;
  }
  return numSent;
}

bool receiveEvent(
    const ShovelerServerNetworkAdapterEvent* event, void* clientConnectionManagerPointer) {
  ShovelerClientConnectionManager* clientConnectionManager = clientConnectionManagerPointer;

  switch (event->type) {
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_NONE:
    break;
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED: {
    if (g_hash_table_contains(clientConnectionManager->clientsByHandle, event->clientHandle)) {
      return false;
    }

    ShovelerClientConnection* clientConnection = malloc(sizeof(ShovelerClientConnection));
    clientConnection->id = clientConnectionManager->nextClientId++;
    clientConnection->handle = event->clientHandle;
    g_hash_table_insert(clientConnectionManager->clients, &clientConnection->id, clientConnection);
    g_hash_table_insert(
        clientConnectionManager->clientsByHandle, clientConnection->handle, clientConnection);

    clientConnectionManager->callbacks->onClientConnected(
        clientConnectionManager,
        clientConnection->id,
        clientConnectionManager->callbacks->userData);

    return true;
  }
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED: {
    ShovelerClientConnection* clientConnection =
        g_hash_table_lookup(clientConnectionManager->clientsByHandle, event->clientHandle);
    if (clientConnection == NULL) {
      return false;
    }

    int64_t clientId = clientConnection->id;

    g_hash_table_remove(clientConnectionManager->clientsByHandle, event->clientHandle);
    g_hash_table_remove(clientConnectionManager->clients, &clientId);

    clientConnectionManager->callbacks->onClientDisconnected(
        clientConnectionManager,
        clientId,
        event->payload->str,
        clientConnectionManager->callbacks->userData);

    return true;
  }
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE: {
    ShovelerClientConnection* clientConnection =
        g_hash_table_lookup(clientConnectionManager->clientsByHandle, event->clientHandle);
    if (clientConnection == NULL) {
      return false;
    }

    int readIndex = 0;
    if (!shovelerServerOpDeserialize(
            clientConnectionManager->serverOp,
            clientConnectionManager->componentTypeIndexer,
            (const unsigned char*) event->payload->str,
            (int) event->payload->len,
            &readIndex)) {
      shovelerLogWarning(
          "Ignoring server op of size %d bytes from client %" PRId64
          " (%p) that failed to deserialize.",
          (int) event->payload->len,
          clientConnection->id,
          clientConnection->handle);
      return false;
    }

    clientConnectionManager->callbacks->onReceiveServerOp(
        clientConnectionManager,
        clientConnection->id,
        &clientConnectionManager->serverOp->op,
        clientConnectionManager->callbacks->userData);
    return true;
  }
  }

  return true;
}

static void freeClientConnection(void* clientConnectionPointer) {
  ShovelerClientConnection* clientConnection = clientConnectionPointer;
  free(clientConnection);
}
