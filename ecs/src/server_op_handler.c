#include "shoveler/server_op_handler.h"

#include <inttypes.h>
#include <shoveler/client_op_emitter.h>
#include <shoveler/client_property_manager.h>
#include <shoveler/component.h>
#include <shoveler/log.h>
#include <shoveler/server_op.h>
#include <shoveler/world.h>
#include <stdlib.h>

ShovelerServerOpHandler shovelerServerOpHandler(
    ShovelerClientPropertyManager* clientPropertyManager,
    ShovelerWorld* world,
    ShovelerClientOpEmitter* clientOpEmitter,
    ShovelerServerOpHandlerAdapter* adapter) {
  ShovelerServerOpHandler serverOpHandler;
  serverOpHandler.clientPropertyManager = clientPropertyManager;
  serverOpHandler.world = world;
  serverOpHandler.clientOpEmitter = clientOpEmitter;
  serverOpHandler.adapter = adapter;
  return serverOpHandler;
}

bool shovelerServerOpHandlerHandle(
    ShovelerServerOpHandler* serverOpHandler, int64_t clientId, const ShovelerServerOp* serverOp) {
  switch (serverOp->type) {
  case SHOVELER_SERVER_OP_NOOP:
    return true;
  case SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST: {
    if (!shovelerClientPropertyManagerAddEntityInterest(
            serverOpHandler->clientPropertyManager,
            clientId,
            serverOp->addEntityInterest.entityId)) {
      shovelerLogWarning(
          "Failed to add interest on entity %lld for client %" PRId64 ": Already interested.",
          serverOp->addEntityInterest.entityId,
          clientId);
      return false;
    }

    ShovelerWorldEntity* entity =
        shovelerWorldGetEntity(serverOpHandler->world, serverOp->addEntityInterest.entityId);
    if (entity != NULL) {
      // The entity already exists, let's check it out.
      shovelerClientOpEmitterCheckoutEntity(serverOpHandler->clientOpEmitter, entity, clientId);
    }
    return true;
  }
  case SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST: {
    if (!shovelerClientPropertyManagerRemoveEntityInterest(
            serverOpHandler->clientPropertyManager,
            clientId,
            serverOp->removeEntityInterest.entityId)) {
      shovelerLogWarning(
          "Failed to remove interest on entity %lld for client %" PRId64 ": Not interested.",
          serverOp->removeEntityInterest.entityId,
          clientId);
      return false;
    }

    ShovelerWorldEntity* entity =
        shovelerWorldGetEntity(serverOpHandler->world, serverOp->removeEntityInterest.entityId);
    if (entity != NULL) {
      // The entity already exists, let's perform an uncheckout.
      shovelerClientOpEmitterUncheckoutEntity(
          serverOpHandler->clientOpEmitter, serverOp->removeEntityInterest.entityId, clientId);
    }
    return true;
  }
  case SHOVELER_SERVER_OP_UPDATE_COMPONENT: {
    if (!serverOpHandler->adapter->hasAuthority(
            serverOpHandler,
            clientId,
            serverOp->updateComponent.entityId,
            serverOp->updateComponent.componentTypeId,
            serverOpHandler->adapter->userData)) {
      char* debugPrint = shovelerServerOpDebugPrint(serverOp);
      shovelerLogWarning(
          "Ignored non-authoritative update from client %" PRId64
          " to entity %lld component %s: %s.",
          clientId,
          serverOp->updateComponent.entityId,
          serverOp->updateComponent.componentTypeId,
          debugPrint);
      free(debugPrint);
      return false;
    }

    ShovelerWorldEntity* entity =
        shovelerWorldGetEntity(serverOpHandler->world, serverOp->updateComponent.entityId);
    if (entity == NULL) {
      char* debugPrint = shovelerServerOpDebugPrint(serverOp);
      shovelerLogWarning(
          "Ignored update from client %" PRId64 " to component %s of non-existing entity %lld: %s.",
          clientId,
          serverOp->updateComponent.componentTypeId,
          serverOp->updateComponent.entityId,
          debugPrint);
      free(debugPrint);
      return false;
    }

    ShovelerComponent* component =
        shovelerWorldEntityGetComponent(entity, serverOp->updateComponent.componentTypeId);
    if (component == NULL) {
      char* debugPrint = shovelerServerOpDebugPrint(serverOp);
      shovelerLogWarning(
          "Ignored update from client %" PRId64 " to non-existing component %s on entity %lld: %s.",
          clientId,
          serverOp->updateComponent.componentTypeId,
          serverOp->updateComponent.entityId,
          debugPrint);
      free(debugPrint);
      return false;
    }

    bool updated = shovelerComponentUpdateField(
        component,
        serverOp->updateComponent.fieldId,
        serverOp->updateComponent.fieldValue,
        /* isCanonical */ true);
    if (!updated) {
      char* debugPrint = shovelerServerOpDebugPrint(serverOp);
      shovelerLogWarning(
          "Failed apply update from client %" PRId64 " to entity %lld component %s: %s.",
          clientId,
          serverOp->updateComponent.entityId,
          serverOp->updateComponent.componentTypeId,
          debugPrint);
      free(debugPrint);
      return false;
    }

    return true;
  }
  default:
    return false;
  }
}
