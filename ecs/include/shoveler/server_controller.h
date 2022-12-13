#ifndef SHOVELER_SERVER_CONTROLLER_H
#define SHOVELER_SERVER_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct ShovelerClientOpEmitterStruct ShovelerClientOpEmitter;
typedef struct ShovelerClientPropertyManagerStruct ShovelerClientPropertyManager;

typedef struct ShovelerServerControllerStruct {
  ShovelerClientOpEmitter* clientOpEmitter;
  ShovelerClientPropertyManager* clientPropertyManager;
} ShovelerServerController;

/** Caller retains ownership over passed objects. */
ShovelerServerController shovelerServerController(
    ShovelerClientOpEmitter* clientOpEmitter, ShovelerClientPropertyManager* clientPropertyManager);
/**
 * Attempts to delegate the given entity component to the specified client.
 *
 * If the entity component is already delegated to another client, an implicit undelegation is
 * performed first. If successful, respective client ops will be emitted for both the original
 * authoritative client and the newly authoritative client if they had interest over the entity.
 */
bool shovelerServerControllerDelegateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);
bool shovelerServerControllerUndelegateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);
bool shovelerServerControllerActivateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);
bool shovelerServerControllerDeactivateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId);

#endif