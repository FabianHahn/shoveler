#ifndef SHOVELER_CLIENT_CONNECTION_MANAGER_H
#define SHOVELER_CLIENT_CONNECTION_MANAGER_H

#include <glib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct ShovelerClientConnectionManagerStruct ShovelerClientConnectionManager;
typedef struct ShovelerClientOpStruct ShovelerClientOp;
typedef struct ShovelerComponentTypeIndexerStruct ShovelerComponentTypeIndexer;
typedef struct ShovelerServerNetworkAdapterStruct ShovelerServerNetworkAdapter;
typedef struct ShovelerServerOpStruct ShovelerServerOp;
typedef struct ShovelerServerOpWithDataStruct ShovelerServerOpWithData;

typedef struct ShovelerClientConnectionManagerCallbacksStruct {
  void (*onClientConnected)(
      ShovelerClientConnectionManager* clientConnectionManager, int64_t clientId, void* userData);
  void (*onClientDisconnected)(
      ShovelerClientConnectionManager* clientConnectionManager,
      int64_t clientId,
      const char* reason,
      void* userData);
  void (*onReceiveServerOp)(
      ShovelerClientConnectionManager* clientConnectionManager,
      int64_t clientId,
      const ShovelerServerOp* serverOp,
      void* userData);

  void* userData;
} ShovelerClientConnectionManagerCallbacks;

typedef struct ShovelerClientConnectionStruct {
  int64_t id;
  void* handle;
} ShovelerClientConnection;

typedef struct ShovelerClientConnectionManagerStruct {
  ShovelerComponentTypeIndexer* componentTypeIndexer;
  ShovelerServerNetworkAdapter* networkAdapter;
  ShovelerClientConnectionManagerCallbacks* callbacks;
  /** map from int64_t client ID to ShovelerClientConnection */
  GHashTable* clients;
  /** map from client handle to ShovelerClientConnection */
  GHashTable* clientsByHandle;
  int64_t nextClientId;
  GString* buffer;
  ShovelerServerOpWithData* serverOp;
} ShovelerClientConnectionManager;

/** Caller retains ownership over passed objects. */
ShovelerClientConnectionManager* shovelerClientConnectionManagerCreate(
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    ShovelerServerNetworkAdapter* networkAdapter,
    ShovelerClientConnectionManagerCallbacks* callbacks);
void shovelerClientConnectionManagerFree(ShovelerClientConnectionManager* clientConnectionManager);
/** Polls and processes incoming events for all client connections. */
int shovelerClientConnectionManagerUpdate(ShovelerClientConnectionManager* clientConnectionManager);
/** Returns number of messages sent to clientProperties. */
int shovelerClientConnectionManagerSendClientOp(
    ShovelerClientConnectionManager* clientConnectionManager,
    const int64_t* clientIds,
    int numClients,
    const ShovelerClientOp* clientOp);

#endif