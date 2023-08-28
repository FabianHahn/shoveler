/**
 * A ClientOpEmitter is responsible for sending ClientOps in response to function calls that
 * notify it of changes to the world. Depending on the event, the client op emitter will then use
 * its adapter to retrieve the list of clients affected by this change. Finally, it will emit ops to
 * the respective affected clients. The change events are:
 *  - an entity being (un)checked out on a particular client
 *  - an entity being added/removed to/from the world
 *  - an entity component being added/removed/updated
 *  - an entity component being delegated/undelegated to/from a particular client
 *
 * In practice, the adapter is usually implemented by:
 *  - getEntityComponents calls are handled by a World
 *  - forwarding prepare* calls that return affected client lists to a ClientPropertyManager
 *  - forwarding onEmitOp callbacks to a ClientConnectionManager
 */

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

typedef void* ShovelerClientOpEmitterAdapterClientDeactivations;

typedef struct ShovelerClientOpEmitterAdapterStruct {
  // World information retrieval
  /** Returns list of (const char *) component type IDs in the output array */
  void (*getEntityComponents)(long long entityId, GArray* componentTypeIdArray, void* userData);

  // Client property retrieval
  void (*prepareEntityInterest)(
      long long int entityId, const char* componentTypeId, GArray* clientIdArray, void* userData);
  void (*prepareClientAuthority)(
      int64_t clientId, long long int entityId, GArray* componentTypeIdArray, void* userData);
  ShovelerClientOpEmitterAdapterClientDeactivations* (*prepareClientDeactivations)(
      int64_t clientId, long long int entityId, void* userData);
  bool (*clientDeactivationsGet)(
      ShovelerClientOpEmitterAdapterClientDeactivations* clientDeactivations,
      const char* componentTypeId,
      void* userData);

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