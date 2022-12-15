#include "shoveler/client_op_emitter.h"

#include <shoveler/client_op.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/world.h>
#include <stdlib.h>

static bool prepareEntityInterest(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId);
static void emitAddEntity(ShovelerClientOpEmitter* clientOpEmitter, ShovelerWorldEntity* entity);
static void emitRemoveEntity(ShovelerClientOpEmitter* clientOpEmitter, long long int entityId);
static void emitAddComponent(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerComponent* component);
static void emitUpdateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentFieldValue* value);
static void emitDelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId);
static void emitUndelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId);
static void emitActivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId);
static void emitDeactivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId);
static void emitRemoveComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId);
static void emitOp(ShovelerClientOpEmitter* clientOpEmitter, const ShovelerClientOp* clientOp);

ShovelerClientOpEmitter* shovelerClientOpEmitterCreate(ShovelerClientOpEmitterAdapter* adapter) {
  ShovelerClientOpEmitter* clientOpEmitter = malloc(sizeof(ShovelerClientOpEmitter));
  clientOpEmitter->adapter = adapter;
  clientOpEmitter->clientIdArray =
      g_array_new(/* zeroTerminated */ false, /* clear */ true, sizeof(int64_t));
  clientOpEmitter->componentTypeIdArray = g_array_new(
      /* zeroTerminated */ false, /* clear */ true, sizeof(const char*));
  return clientOpEmitter;
}

void shovelerClientOpEmitterFree(ShovelerClientOpEmitter* clientOpEmitter) {
  g_array_free(clientOpEmitter->componentTypeIdArray, /* freeSegment */ true);
  g_array_free(clientOpEmitter->clientIdArray, /* freeSegment */ true);
  free(clientOpEmitter);
}

void shovelerClientOpEmitterCheckoutEntity(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerWorldEntity* entity, int64_t clientId) {
  g_array_set_size(clientOpEmitter->clientIdArray, 1);
  g_array_index(clientOpEmitter->clientIdArray, int64_t, 0) = clientId;

  emitAddEntity(clientOpEmitter, entity);

  clientOpEmitter->adapter->prepareClientAuthority(
      clientId,
      entity->id,
      clientOpEmitter->componentTypeIdArray,
      clientOpEmitter->adapter->userData);
  for (int i = 0; i < clientOpEmitter->componentTypeIdArray->len; i++) {
    const char* componentTypeId =
        g_array_index(clientOpEmitter->componentTypeIdArray, const char*, i);
    emitDelegateComponent(clientOpEmitter, entity->id, componentTypeId);
  }

  GHashTableIter iter;
  ShovelerComponent* component;
  g_hash_table_iter_init(&iter, entity->components);
  while (g_hash_table_iter_next(&iter, /* key */ NULL, (gpointer*) &component)) {
    emitAddComponent(clientOpEmitter, component);

    for (int fieldId = 0; fieldId < component->type->numFields; fieldId++) {
      emitUpdateComponent(clientOpEmitter, component, fieldId, &component->fieldValues[fieldId]);
    }
  }

  clientOpEmitter->adapter->prepareClientActivations(
      clientId,
      entity->id,
      clientOpEmitter->componentTypeIdArray,
      clientOpEmitter->adapter->userData);
  for (int i = 0; i < clientOpEmitter->componentTypeIdArray->len; i++) {
    const char* componentTypeId =
        g_array_index(clientOpEmitter->componentTypeIdArray, const char*, i);
    emitActivateComponent(clientOpEmitter, entity->id, componentTypeId);
  }
}

void shovelerClientOpEmitterUncheckoutEntity(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, int64_t clientId) {
  g_array_set_size(clientOpEmitter->clientIdArray, 1);
  g_array_index(clientOpEmitter->clientIdArray, int64_t, 0) = clientId;
  emitRemoveEntity(clientOpEmitter, entityId);
}

void shovelerClientOpEmitterAddEntity(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerWorldEntity* entity) {
  if (!prepareEntityInterest(clientOpEmitter, entity->id, /* componentTypeId */ NULL)) {
    return;
  }

  emitAddEntity(clientOpEmitter, entity);
}

void shovelerClientOpEmitterRemoveEntity(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId) {
  if (!prepareEntityInterest(clientOpEmitter, entityId, /* componentTypeId */ NULL)) {
    return;
  }

  emitRemoveEntity(clientOpEmitter, entityId);
}

void shovelerClientOpEmitterAddComponent(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerComponent* component) {
  if (!prepareEntityInterest(clientOpEmitter, component->entityId, component->type->id)) {
    return;
  }

  emitAddComponent(clientOpEmitter, component);
}

void shovelerClientOpEmitterUpdateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentFieldValue* value) {
  if (!prepareEntityInterest(clientOpEmitter, component->entityId, component->type->id)) {
    return;
  }

  emitUpdateComponent(clientOpEmitter, component, fieldId, value);
}

void shovelerClientOpEmitterDelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  g_array_set_size(clientOpEmitter->clientIdArray, 1);
  g_array_index(clientOpEmitter->clientIdArray, int64_t, 0) = clientId;
  emitDelegateComponent(clientOpEmitter, entityId, componentTypeId);
}

void shovelerClientOpEmitterUndelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  g_array_set_size(clientOpEmitter->clientIdArray, 1);
  g_array_index(clientOpEmitter->clientIdArray, int64_t, 0) = clientId;
  emitUndelegateComponent(clientOpEmitter, entityId, componentTypeId);
}

void shovelerClientOpEmitterActivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  g_array_set_size(clientOpEmitter->clientIdArray, 1);
  g_array_index(clientOpEmitter->clientIdArray, int64_t, 0) = clientId;
  emitActivateComponent(clientOpEmitter, entityId, componentTypeId);
}

void shovelerClientOpEmitterDeactivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  g_array_set_size(clientOpEmitter->clientIdArray, 1);
  g_array_index(clientOpEmitter->clientIdArray, int64_t, 0) = clientId;
  emitDeactivateComponent(clientOpEmitter, entityId, componentTypeId);
}

void shovelerClientOpEmitterRemoveComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId) {
  if (!prepareEntityInterest(clientOpEmitter, entityId, componentTypeId)) {
    return;
  }

  emitRemoveComponent(clientOpEmitter, entityId, componentTypeId);
}

static bool prepareEntityInterest(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId) {
  clientOpEmitter->adapter->prepareEntityInterest(
      entityId,
      componentTypeId,
      clientOpEmitter->clientIdArray,
      clientOpEmitter->adapter->userData);
  return clientOpEmitter->clientIdArray->len > 0;
}

static void emitAddEntity(ShovelerClientOpEmitter* clientOpEmitter, ShovelerWorldEntity* entity) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_ADD_ENTITY;
  clientOp.addEntity.entityId = entity->id;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitRemoveEntity(ShovelerClientOpEmitter* clientOpEmitter, long long int entityId) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_REMOVE_ENTITY;
  clientOp.removeEntity.entityId = entityId;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitAddComponent(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerComponent* component) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_ADD_COMPONENT;
  clientOp.addComponent.entityId = component->entityId;
  clientOp.addComponent.componentTypeId = component->type->id;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitUpdateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentFieldValue* value) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_UPDATE_COMPONENT;
  clientOp.updateComponent.entityId = component->entityId;
  clientOp.updateComponent.componentTypeId = component->type->id;
  clientOp.updateComponent.fieldId = fieldId;
  clientOp.updateComponent.fieldValue = value;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitDelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_DELEGATE_COMPONENT;
  clientOp.delegateComponent.entityId = entityId;
  clientOp.delegateComponent.componentTypeId = componentTypeId;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitUndelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT;
  clientOp.undelegateComponent.entityId = entityId;
  clientOp.undelegateComponent.componentTypeId = componentTypeId;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitActivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT;
  clientOp.activateComponent.entityId = entityId;
  clientOp.activateComponent.componentTypeId = componentTypeId;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitDeactivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT;
  clientOp.deactivateComponent.entityId = entityId;
  clientOp.deactivateComponent.componentTypeId = componentTypeId;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitRemoveComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId) {
  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_REMOVE_COMPONENT;
  clientOp.removeComponent.entityId = entityId;
  clientOp.removeComponent.componentTypeId = componentTypeId;
  emitOp(clientOpEmitter, &clientOp);
}

static void emitOp(ShovelerClientOpEmitter* clientOpEmitter, const ShovelerClientOp* clientOp) {
  clientOpEmitter->adapter->onEmitOp(
      clientOpEmitter,
      (const int64_t*) clientOpEmitter->clientIdArray->data,
      (int) clientOpEmitter->clientIdArray->len,
      clientOp,
      clientOpEmitter->adapter->userData);
}
