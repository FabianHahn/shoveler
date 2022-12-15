#ifndef SHOVELER_CLIENT_OP_EMITTER_H
#define SHOVELER_CLIENT_OP_EMITTER_H

#include <glib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct ShovelerClientOpStruct ShovelerClientOp;
typedef struct ShovelerComponentFieldStruct ShovelerComponentField;
typedef struct ShovelerComponentFieldValueStruct ShovelerComponentFieldValue;
typedef struct ShovelerComponentStruct ShovelerComponent;
typedef struct ShovelerClientOpEmitterStruct ShovelerClientOpEmitter;
typedef struct ShovelerWorldEntityStruct ShovelerWorldEntity;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef struct ShovelerClientOpEmitterAdapterStruct {
  void (*prepareEntityInterest)(
      long long int entityId, const char* componentTypeId, GArray* clientIdArray, void* userData);
  void (*prepareClientAuthority)(
      int64_t clientId, long long int entityId, GArray* componentTypeIdArray, void* userData);
  void (*prepareClientActivations)(
      int64_t clientId, long long int entityId, GArray* componentTypeIdArray, void* userData);

  void (*onEmitOp)(
      ShovelerClientOpEmitter* clientOpEmitter,
      const int64_t* clientIds,
      int numClients,
      const ShovelerClientOp* clientOp,
      void* userData);

  void* userData;
} ShovelerClientOpEmitterAdapter;

typedef struct ShovelerClientOpEmitterStruct {
  ShovelerClientOpEmitterAdapter* adapter;
  GArray* clientIdArray;
  GArray* componentTypeIdArray;
} ShovelerClientOpEmitter;

/** Caller retains ownership over passed objects. */
ShovelerClientOpEmitter* shovelerClientOpEmitterCreate(ShovelerClientOpEmitterAdapter* adapter);
void shovelerClientOpEmitterFree(ShovelerClientOpEmitter* clientOpEmitter);
void shovelerClientOpEmitterCheckoutEntity(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerWorldEntity* entity, int64_t clientId);
void shovelerClientOpEmitterUncheckoutEntity(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, int64_t clientId);
void shovelerClientOpEmitterAddEntity(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerWorldEntity* entity);
void shovelerClientOpEmitterRemoveEntity(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId);
void shovelerClientOpEmitterAddComponent(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerComponent* component);
void shovelerClientOpEmitterUpdateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentFieldValue* value);
void shovelerClientOpEmitterDelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);
void shovelerClientOpEmitterUndelegateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);
void shovelerClientOpEmitterActivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);
void shovelerClientOpEmitterDeactivateComponent(
    ShovelerClientOpEmitter* clientOpEmitter,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);
void shovelerClientOpEmitterRemoveComponent(
    ShovelerClientOpEmitter* clientOpEmitter, long long int entityId, const char* componentTypeId);

#endif