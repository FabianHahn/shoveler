#include "shoveler/view_synchronizer.h"

#include <assert.h>
#include <shoveler/client_property_manager.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/component_type_indexer.h>
#include <shoveler/log.h>
#include <shoveler/schema.h>
#include <stdlib.h>

static void onAddEntity(ShovelerWorld* world, ShovelerWorldEntity* entity, void* userData);
static void onRemoveEntity(ShovelerWorld* world, long long int entityId, void* userData);
static void onAddComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* userData);
static void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* userData);
static void onRemoveComponent(
    ShovelerWorld* world, ShovelerWorldEntity* entity, const char* componentTypeId, void* userData);
static void prepareEntityInterest(
    long long int entityId, const char* componentTypeId, GArray* clientIdArray, void* userData);
static void prepareClientAuthority(
    int64_t clientId, long long int entityId, GArray* componentTypeIdArray, void* userData);
static void prepareClientActivations(
    int64_t clientId,
    long long int entityId,
    GArray* componentTypeIdArray,
    void* viewSynchronizerPointer);
static void onEmitOp(
    ShovelerClientOpEmitter* clientOpEmitter,
    const int64_t* clientIds,
    int numClients,
    const ShovelerClientOp* clientOp,
    void* userData);
static void onClientConnected(
    ShovelerClientConnectionManager* clientConnectionManager, int64_t clientId, void* userData);
static void onClientDisconnected(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const char* reason,
    void* userData);
static void onReceiveServerOp(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const ShovelerServerOp* serverOp,
    void* userData);
static bool hasAuthority(
    ShovelerServerOpHandler* serverOpHandler,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId,
    void* userData);

ShovelerViewSynchronizer* shovelerViewSynchronizerCreate(
    ShovelerSchema* schema,
    ShovelerSystem* system,
    ShovelerServerNetworkAdapter* networkAdapter,
    ShovelerViewSynchronizerCallbacks* callbacks) {
  ShovelerViewSynchronizer* viewSynchronizer = malloc(sizeof(ShovelerViewSynchronizer));

  viewSynchronizer->worldCallbacks = shovelerWorldCallbacks();
  viewSynchronizer->worldCallbacks.onAddEntity = onAddEntity;
  viewSynchronizer->worldCallbacks.onRemoveEntity = onRemoveEntity;
  viewSynchronizer->worldCallbacks.onAddComponent = onAddComponent;
  viewSynchronizer->worldCallbacks.onUpdateComponent = onUpdateComponent;
  viewSynchronizer->worldCallbacks.onRemoveComponent = onRemoveComponent;
  viewSynchronizer->worldCallbacks.userData = viewSynchronizer;
  viewSynchronizer->world = shovelerWorldCreate(schema, system, &viewSynchronizer->worldCallbacks);

  viewSynchronizer->clientPropertyManager = shovelerClientPropertyManagerCreate();

  viewSynchronizer->clientOpEmitterAdapter.prepareEntityInterest = prepareEntityInterest;
  viewSynchronizer->clientOpEmitterAdapter.prepareClientAuthority = prepareClientAuthority;
  viewSynchronizer->clientOpEmitterAdapter.prepareClientActivations = prepareClientActivations;
  viewSynchronizer->clientOpEmitterAdapter.onEmitOp = onEmitOp;
  viewSynchronizer->clientOpEmitterAdapter.userData = viewSynchronizer;
  viewSynchronizer->clientOpEmitter =
      shovelerClientOpEmitterCreate(&viewSynchronizer->clientOpEmitterAdapter);

  viewSynchronizer->componentTypeIndexer = shovelerComponentTypeIndexerCreate();
  // TODO: stable order for these so they match between client and server side
  GHashTableIter iter;
  const char* componentTypeId;
  g_hash_table_iter_init(&iter, schema->componentTypes);
  while (g_hash_table_iter_next(&iter, (gpointer*) &componentTypeId, /* value */ NULL)) {
    bool added = shovelerComponentTypeIndexerAddComponentType(
        viewSynchronizer->componentTypeIndexer, componentTypeId);
    assert(added);
  }

  viewSynchronizer->clientConnectionManagerCallbacks.onClientConnected = onClientConnected;
  viewSynchronizer->clientConnectionManagerCallbacks.onClientDisconnected = onClientDisconnected;
  viewSynchronizer->clientConnectionManagerCallbacks.onReceiveServerOp = onReceiveServerOp;
  viewSynchronizer->clientConnectionManagerCallbacks.userData = viewSynchronizer;
  viewSynchronizer->clientConnectionManager = shovelerClientConnectionManagerCreate(
      viewSynchronizer->componentTypeIndexer,
      networkAdapter,
      &viewSynchronizer->clientConnectionManagerCallbacks);

  viewSynchronizer->serverOpHandlerAdapter.hasAuthority = hasAuthority;
  viewSynchronizer->serverOpHandlerAdapter.userData = viewSynchronizer;
  viewSynchronizer->serverOpHandler = shovelerServerOpHandler(
      viewSynchronizer->clientPropertyManager,
      viewSynchronizer->world,
      viewSynchronizer->clientOpEmitter,
      &viewSynchronizer->serverOpHandlerAdapter);

  viewSynchronizer->serverController = shovelerServerController(
      viewSynchronizer->clientOpEmitter, viewSynchronizer->clientPropertyManager);

  viewSynchronizer->callbacks = callbacks;

  return viewSynchronizer;
}

void shovelerViewSynchronizerFree(ShovelerViewSynchronizer* viewSynchronizer) {
  // World needs to be freed first as it will result in callbacks to the other objects.
  shovelerWorldFree(viewSynchronizer->world);
  shovelerClientConnectionManagerFree(viewSynchronizer->clientConnectionManager);
  shovelerComponentTypeIndexerFree(viewSynchronizer->componentTypeIndexer);
  shovelerClientOpEmitterFree(viewSynchronizer->clientOpEmitter);
  shovelerClientPropertyManagerFree(viewSynchronizer->clientPropertyManager);
  free(viewSynchronizer);
}

ShovelerServerController* shovelerViewSynchronizerGetServerController(
    ShovelerViewSynchronizer* viewSynchronizer) {
  return &viewSynchronizer->serverController;
}

ShovelerWorld* shovelerViewSynchronizerGetWorld(ShovelerViewSynchronizer* viewSynchronizer) {
  return viewSynchronizer->world;
}

void shovelerViewSynchronizerUpdate(ShovelerViewSynchronizer* viewSynchronizer) {
  shovelerClientConnectionManagerUpdate(viewSynchronizer->clientConnectionManager);
}

static void onAddEntity(
    ShovelerWorld* world, ShovelerWorldEntity* entity, void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientOpEmitterAddEntity(viewSynchronizer->clientOpEmitter, entity);
}

static void onRemoveEntity(
    ShovelerWorld* world, long long int entityId, void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientOpEmitterRemoveEntity(viewSynchronizer->clientOpEmitter, entityId);
}

static void onAddComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientOpEmitterAddComponent(viewSynchronizer->clientOpEmitter, component);
}

static void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientOpEmitterUpdateComponent(
      viewSynchronizer->clientOpEmitter, component, fieldId, value);
}

static void onRemoveComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    const char* componentTypeId,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientOpEmitterRemoveComponent(
      viewSynchronizer->clientOpEmitter, entity->id, componentTypeId);
}

static void prepareEntityInterest(
    long long int entityId,
    const char* componentTypeId,
    GArray* clientIdArray,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientPropertyManagerGetEntityInterest(
      viewSynchronizer->clientPropertyManager, entityId, componentTypeId, clientIdArray);
}

static void prepareClientAuthority(
    int64_t clientId,
    long long int entityId,
    GArray* componentTypeIdArray,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientPropertyManagerGetClientAuthority(
      viewSynchronizer->clientPropertyManager, clientId, entityId, componentTypeIdArray);
}

static void prepareClientActivations(
    int64_t clientId,
    long long int entityId,
    GArray* componentTypeIdArray,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientPropertyManagerGetClientActivations(
      viewSynchronizer->clientPropertyManager, clientId, entityId, componentTypeIdArray);
}

static void onEmitOp(
    ShovelerClientOpEmitter* clientOpEmitter,
    const int64_t* clientIds,
    int numClients,
    const ShovelerClientOp* clientOp,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientConnectionManagerSendClientOp(
      viewSynchronizer->clientConnectionManager, clientIds, numClients, clientOp);
}

static void onClientConnected(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientPropertyManagerAddClient(viewSynchronizer->clientPropertyManager, clientId);

  viewSynchronizer->callbacks->onClientConnected(
      viewSynchronizer, clientId, viewSynchronizer->callbacks->userData);
}

static void onClientDisconnected(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const char* reason,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerClientPropertyManagerRemoveClient(viewSynchronizer->clientPropertyManager, clientId);

  viewSynchronizer->callbacks->onClientDisconnected(
      viewSynchronizer, clientId, reason, viewSynchronizer->callbacks->userData);
}

static void onReceiveServerOp(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const ShovelerServerOp* serverOp,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  shovelerServerOpHandlerHandle(&viewSynchronizer->serverOpHandler, clientId, serverOp);
}

static bool hasAuthority(
    ShovelerServerOpHandler* serverOpHandler,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId,
    void* viewSynchronizerPointer) {
  ShovelerViewSynchronizer* viewSynchronizer = viewSynchronizerPointer;
  return shovelerClientPropertyManagerHasComponentAuthority(
      viewSynchronizer->clientPropertyManager, clientId, entityId, componentTypeId);
}