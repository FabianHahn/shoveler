#include "shoveler/in_memory_network_adapter.h"

#include <shoveler/log.h>
#include <stdlib.h>

static bool serverSendMessage(
    void* clientHandle, const unsigned char* data, int size, void* inMemoryNetworkAdapterPointer);
static bool serverReceiveEvent(
    bool (*receiveEventCallback)(const ShovelerServerNetworkAdapterEvent* event, void* userData),
    void* callbackUserData,
    void* inMemoryNetworkAdapterPointer);
static bool clientSendMessage(const unsigned char* data, int size, void* clientPointer);
static bool clientReceiveEvent(
    bool (*receiveEventCallback)(const ShovelerClientNetworkAdapterEvent* event, void* userData),
    void* callbackUserData,
    void* clientPointer);
static void freeClient(void* clientPointer);
static void clearServerEvent(void* serverEventPointer);
static void clearClientEvent(void* clientEventPointer);

ShovelerInMemoryNetworkAdapter* shovelerInMemoryNetworkAdapterCreate() {
  ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter =
      malloc(sizeof(ShovelerInMemoryNetworkAdapter));
  inMemoryNetworkAdapter->clients =
      g_hash_table_new_full(g_direct_hash, g_direct_equal, /* keyDestroyFunc */ NULL, freeClient);
  inMemoryNetworkAdapter->serverEvents = g_array_new(
      /* zeroTerminated */ false, /* clear */ true, sizeof(ShovelerServerNetworkAdapterEvent));
  g_array_set_clear_func(inMemoryNetworkAdapter->serverEvents, clearServerEvent);
  inMemoryNetworkAdapter->serverNetworkAdapter.sendMessage = serverSendMessage;
  inMemoryNetworkAdapter->serverNetworkAdapter.receiveEvent = serverReceiveEvent;
  inMemoryNetworkAdapter->serverNetworkAdapter.userData = inMemoryNetworkAdapter;
  inMemoryNetworkAdapter->nextEventIndex = 0;
  return inMemoryNetworkAdapter;
}

void shovelerInMemoryNetworkAdapterFree(ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter) {
  g_hash_table_destroy(inMemoryNetworkAdapter->clients);
  g_array_free(inMemoryNetworkAdapter->serverEvents, /* freeSegment */ true);
  free(inMemoryNetworkAdapter);
}

ShovelerServerNetworkAdapter* shovelerInMemoryNetworkAdapterGetServer(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter) {
  return &inMemoryNetworkAdapter->serverNetworkAdapter;
}

void* shovelerInMemoryNetworkAdapterConnectClient(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter) {
  ShovelerInMemoryNetworkAdapterClient* client =
      malloc(sizeof(ShovelerInMemoryNetworkAdapterClient));
  client->inMemoryNetworkAdapter = inMemoryNetworkAdapter;
  client->clientNetworkAdapter.sendMessage = clientSendMessage;
  client->clientNetworkAdapter.receiveEvent = clientReceiveEvent;
  client->clientNetworkAdapter.userData = client;
  client->isDisconnecting = false;
  client->clientEvents = g_array_new(
      /* zeroTerminated */ false, /* clear */ true, sizeof(ShovelerClientNetworkAdapterEvent));
  g_array_set_clear_func(client->clientEvents, clearClientEvent);
  client->nextEventIndex = 0;

  g_hash_table_add(inMemoryNetworkAdapter->clients, client);

  ShovelerServerNetworkAdapterEvent serverEvent =
      shovelerServerNetworkAdapterEventClientConnected(client);
  g_array_append_val(inMemoryNetworkAdapter->serverEvents, serverEvent);

  ShovelerClientNetworkAdapterEvent clientEvent =
      shovelerClientNetworkAdapterEventClientConnected();
  g_array_append_val(client->clientEvents, clientEvent);

  return client;
}

bool shovelerInMemoryNetworkAdapterDisconnectClient(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter,
    void* clientHandle,
    const char* reason) {
  ShovelerInMemoryNetworkAdapterClient* client =
      g_hash_table_lookup(inMemoryNetworkAdapter->clients, clientHandle);
  if (client == NULL) {
    return false;
  }

  if (client->isDisconnecting) {
    return false;
  }

  client->isDisconnecting = true;

  ShovelerClientNetworkAdapterEvent clientEvent =
      shovelerClientNetworkAdapterEventClientDisconnected(reason);
  g_array_append_val(client->clientEvents, clientEvent);

  ShovelerServerNetworkAdapterEvent serverEvent =
      shovelerServerNetworkAdapterEventClientDisconnected(client, reason);
  g_array_append_val(inMemoryNetworkAdapter->serverEvents, serverEvent);

  return true;
}

ShovelerClientNetworkAdapter* shovelerInMemoryNetworkAdapterGetClient(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter, void* clientHandle) {
  ShovelerInMemoryNetworkAdapterClient* client =
      g_hash_table_lookup(inMemoryNetworkAdapter->clients, clientHandle);
  if (client == NULL) {
    return false;
  }

  return &client->clientNetworkAdapter;
}

static bool serverSendMessage(
    void* clientHandle, const unsigned char* data, int size, void* inMemoryNetworkAdapterPointer) {
  ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter = inMemoryNetworkAdapterPointer;

  ShovelerInMemoryNetworkAdapterClient* client =
      g_hash_table_lookup(inMemoryNetworkAdapter->clients, clientHandle);
  if (client == NULL) {
    shovelerLogWarning(
        "Ignoring message of size %d being sent from unknown client %p.", size, clientHandle);
    return false;
  }

  if (client->isDisconnecting) {
    shovelerLogWarning(
        "Dropping message of size %d to disconnecting client %p.", size, clientHandle);
    return false;
  }

  ShovelerClientNetworkAdapterEvent clientEvent =
      shovelerClientNetworkAdapterEventMessage(data, size);
  g_array_append_val(client->clientEvents, clientEvent);
  return true;
}

static bool serverReceiveEvent(
    bool (*receiveEventCallback)(const ShovelerServerNetworkAdapterEvent* event, void* userData),
    void* callbackUserData,
    void* inMemoryNetworkAdapterPointer) {
  ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter = inMemoryNetworkAdapterPointer;
  if (inMemoryNetworkAdapter->serverEvents->len == 0) {
    return false;
  }

  ShovelerServerNetworkAdapterEvent* nextEvent = &g_array_index(
      inMemoryNetworkAdapter->serverEvents,
      ShovelerServerNetworkAdapterEvent,
      inMemoryNetworkAdapter->nextEventIndex);
  bool result = receiveEventCallback(nextEvent, callbackUserData);

  inMemoryNetworkAdapter->nextEventIndex++;
  if (inMemoryNetworkAdapter->nextEventIndex == inMemoryNetworkAdapter->serverEvents->len) {
    // All events have been read, clear the queue.
    g_array_set_size(inMemoryNetworkAdapter->serverEvents, 0);
    inMemoryNetworkAdapter->nextEventIndex = 0;
  }
  return result;
}

static bool clientSendMessage(const unsigned char* data, int size, void* clientPointer) {
  ShovelerInMemoryNetworkAdapterClient* client = clientPointer;

  ShovelerServerNetworkAdapterEvent serverEvent =
      shovelerServerNetworkAdapterEventMessage(client, data, size);
  g_array_append_val(client->inMemoryNetworkAdapter->serverEvents, serverEvent);
  return true;
}

static bool clientReceiveEvent(
    bool (*receiveEventCallback)(const ShovelerClientNetworkAdapterEvent* event, void* userData),
    void* callbackUserData,
    void* clientPointer) {
  ShovelerInMemoryNetworkAdapterClient* client = clientPointer;
  if (client->clientEvents->len == 0) {
    if (client->isDisconnecting) {
      g_hash_table_remove(client->inMemoryNetworkAdapter->clients, client);
    }

    return false;
  }

  ShovelerClientNetworkAdapterEvent* nextEvent = &g_array_index(
      client->clientEvents, ShovelerClientNetworkAdapterEvent, client->nextEventIndex);
  bool result = receiveEventCallback(nextEvent, callbackUserData);

  client->nextEventIndex++;
  if (client->nextEventIndex == client->clientEvents->len) {
    // All events have been read, clear the queue.
    g_array_set_size(client->clientEvents, 0);
    client->nextEventIndex = 0;
  }
  return result;
}

static void freeClient(void* clientPointer) {
  ShovelerInMemoryNetworkAdapterClient* client = clientPointer;
  g_array_free(client->clientEvents, /* freeSegment */ true);
  free(client);
}

static void clearServerEvent(void* serverEventPointer) {
  ShovelerServerNetworkAdapterEvent* serverEvent = serverEventPointer;
  shovelerServerNetworkAdapterEventClear(serverEvent);
}

static void clearClientEvent(void* clientEventPointer) {
  ShovelerClientNetworkAdapterEvent* clientEvent = clientEventPointer;
  shovelerClientNetworkAdapterEventClear(clientEvent);
}
