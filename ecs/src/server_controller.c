#include "shoveler/server_controller.h"

#include <shoveler/client_op_emitter.h>
#include <shoveler/client_property_manager.h>

ShovelerServerController shovelerServerController(
    ShovelerClientOpEmitter* clientOpEmitter,
    ShovelerClientPropertyManager* clientPropertyManager) {
  ShovelerServerController serverController;
  serverController.clientOpEmitter = clientOpEmitter;
  serverController.clientPropertyManager = clientPropertyManager;
  return serverController;
}

bool shovelerServerControllerDelegateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  int64_t authoritativeClientId;
  if (shovelerClientPropertyManagerGetComponentAuthority(
          serverController->clientPropertyManager,
          entityId,
          componentTypeId,
          &authoritativeClientId)) {
    // Already delegated, attempt to undelegate it first.
    if (!shovelerServerControllerUndelegateComponent(
            serverController, entityId, componentTypeId, authoritativeClientId)) {
      return false;
    }
  }

  if (!shovelerClientPropertyManagerAddComponentAuthority(
          serverController->clientPropertyManager, clientId, entityId, componentTypeId)) {
    return false;
  }

  if (shovelerClientPropertyManagerHasEntityInterest(
          serverController->clientPropertyManager, clientId, entityId)) {
    // Entity has interest, we need to send a delegate op.
    shovelerClientOpEmitterDelegateComponent(
        serverController->clientOpEmitter, entityId, componentTypeId, clientId);
  }
  return true;
}

bool shovelerServerControllerUndelegateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  if (!shovelerClientPropertyManagerHasComponentAuthority(
          serverController->clientPropertyManager, clientId, entityId, componentTypeId)) {
    return false;
  }

  if (!shovelerClientPropertyManagerRemoveComponentAuthority(
          serverController->clientPropertyManager, clientId, entityId, componentTypeId)) {
    return false;
  }

  if (shovelerClientPropertyManagerHasEntityInterest(
          serverController->clientPropertyManager, clientId, entityId)) {
    // Entity has interest, we need to send an undelegate op.
    shovelerClientOpEmitterUndelegateComponent(
        serverController->clientOpEmitter, entityId, componentTypeId, clientId);
  }
  return true;
}

bool shovelerServerControllerDeactivateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  if (!shovelerClientPropertyManagerAddComponentDeactivation(
          serverController->clientPropertyManager, clientId, entityId, componentTypeId)) {
    return false;
  }

  if (shovelerClientPropertyManagerHasEntityInterest(
          serverController->clientPropertyManager, clientId, entityId)) {
    // Entity has interest, we need to send a deactivate op.
    shovelerClientOpEmitterDeactivateComponent(
        serverController->clientOpEmitter, entityId, componentTypeId, clientId);
  }
  return true;
}

bool shovelerServerControllerUndeactivateComponent(
    ShovelerServerController* serverController,
    long long int entityId,
    const char* componentTypeId,
    int64_t clientId) {
  if (!shovelerClientPropertyManagerRemoveComponentDeactivation(
          serverController->clientPropertyManager, clientId, entityId, componentTypeId)) {
    return false;
  }

  if (shovelerClientPropertyManagerHasEntityInterest(
          serverController->clientPropertyManager, clientId, entityId)) {
    // Entity has interest, we need to send an activate op.
    shovelerClientOpEmitterActivateComponent(
        serverController->clientOpEmitter, entityId, componentTypeId, clientId);
  }
  return true;
}
