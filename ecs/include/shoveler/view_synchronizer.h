#ifndef SHOVELER_VIEW_SYNCHRONIZER_H
#define SHOVELER_VIEW_SYNCHRONIZER_H

#include <shoveler/client_connection_manager.h>
#include <shoveler/client_op_emitter.h>
#include <shoveler/server_controller.h>
#include <shoveler/server_op_handler.h>
#include <shoveler/world.h>

typedef struct ShovelerClientConnectionManagerCallbacksStruct
    ShovelerClientConnectionManagerCallbacks;
typedef struct ShovelerClientConnectionManagerStruct ShovelerClientConnectionManager;
typedef struct ShovelerClientPropertyManagerStruct ShovelerClientPropertyManager;
typedef struct ShovelerComponentTypeIndexerStruct ShovelerComponentTypeIndexer;
typedef struct ShovelerServerNetworkAdapterStruct ShovelerServerNetworkAdapter;
typedef struct ShovelerViewSynchronizerStruct ShovelerViewSynchronizer;

typedef struct ShovelerViewSynchronizerCallbacksStruct {
  void (*onClientConnected)(
      ShovelerViewSynchronizer* viewSynchronizer, int64_t clientId, void* userData);
  void (*onClientDisconnected)(
      ShovelerViewSynchronizer* viewSynchronizer,
      int64_t clientId,
      const char* reason,
      void* userData);

  void* userData;
} ShovelerViewSynchronizerCallbacks;

typedef struct ShovelerViewSynchronizerStruct {
  ShovelerWorldCallbacks worldCallbacks;
  ShovelerWorld* world;
  ShovelerClientPropertyManager* clientPropertyManager;
  ShovelerClientOpEmitterAdapter clientOpEmitterAdapter;
  ShovelerClientOpEmitter* clientOpEmitter;
  ShovelerComponentTypeIndexer* componentTypeIndexer;
  ShovelerClientConnectionManagerCallbacks clientConnectionManagerCallbacks;
  ShovelerClientConnectionManager* clientConnectionManager;
  ShovelerServerOpHandlerAdapter serverOpHandlerAdapter;
  ShovelerServerOpHandler serverOpHandler;
  ShovelerServerController serverController;
  ShovelerViewSynchronizerCallbacks* callbacks;
} ShovelerViewSynchronizer;

ShovelerViewSynchronizer* shovelerViewSynchronizerCreate(
    ShovelerSchema* schema,
    ShovelerSystem* system,
    ShovelerServerNetworkAdapter* networkAdapter,
    ShovelerViewSynchronizerCallbacks* callbacks);
void shovelerViewSynchronizerFree(ShovelerViewSynchronizer* viewSynchronizer);
ShovelerServerController* shovelerViewSynchronizerGetServerController(
    ShovelerViewSynchronizer* viewSynchronizer);
ShovelerWorld* shovelerViewSynchronizerGetWorld(ShovelerViewSynchronizer* viewSynchronizer);
void shovelerViewSynchronizerUpdate(ShovelerViewSynchronizer* viewSynchronizer);

#endif