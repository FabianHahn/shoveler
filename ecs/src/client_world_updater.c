#include "shoveler/client_world_updater.h"

#include "shoveler/client_op.h"
#include "shoveler/component.h"
#include "shoveler/world.h"

ShovelerClientWorldUpdaterStatus shovelerClientWorldUpdaterApplyOp(
    ShovelerClientWorldUpdater* clientWorldUpdater, const ShovelerClientOp* clientOp) {
  ShovelerWorld* world = clientWorldUpdater->world;

  switch (clientOp->type) {
  case SHOVELER_CLIENT_OP_NOOP:
    return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
  case SHOVELER_CLIENT_OP_ADD_ENTITY: {
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, clientOp->addEntity.entityId);
    if (entity == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_ALREADY_EXISTS;
    }
    return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
  }
  case SHOVELER_CLIENT_OP_REMOVE_ENTITY: {
    if (!shovelerWorldRemoveEntity(world, clientOp->removeEntity.entityId)) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST;
    }
    return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
  }
  case SHOVELER_CLIENT_OP_ADD_COMPONENT: {
    ShovelerWorldEntity* entity = shovelerWorldGetEntity(world, clientOp->addComponent.entityId);
    if (entity == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST;
    }

    ShovelerWorldEntityAddComponentStatus status;
    shovelerWorldEntityAddComponent(entity, clientOp->addComponent.componentTypeId, &status);
    switch (status) {
    case SHOVELER_WORLD_ENTITY_ADD_COMPONENT_SUCCESS:
      return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
    case SHOVELER_WORLD_ENTITY_ADD_COMPONENT_INVALID_TYPE:
      return SHOVELER_CLIENT_WORLD_UPDATER_INVALID_COMPONENT_TYPE;
    case SHOVELER_WORLD_ENTITY_ADD_COMPONENT_ALREADY_EXISTS:
      return SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_ALREADY_EXISTS;
    default:
      assert(false);
    }
  }
  case SHOVELER_CLIENT_OP_UPDATE_COMPONENT: {
    ShovelerWorldEntity* entity = shovelerWorldGetEntity(world, clientOp->updateComponent.entityId);
    if (entity == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST;
    }

    ShovelerComponent* component =
        shovelerWorldEntityGetComponent(entity, clientOp->updateComponent.componentTypeId);
    if (component == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_DOESNT_EXISTS;
    }

    ShovelerComponentUpdateFieldStatus status = shovelerComponentUpdateField(
        component,
        clientOp->updateComponent.fieldId,
        clientOp->updateComponent.fieldValue,
        /* isAuthoritative */ true);
    switch (status) {
    case SHOVELER_COMPONENT_UPDATE_FIELD_SUCCESS:
      return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
    case SHOVELER_COMPONENT_UPDATE_FIELD_TYPE_MISMATCH:
      return SHOVELER_CLIENT_WORLD_UPDATER_INVALID_FIELD_TYPE;
    case SHOVELER_COMPONENT_UPDATE_FIELD_NOT_AUTHORITATIVE:
      return SHOVELER_CLIENT_WORLD_UPDATER_NOT_AUTHORITATIVE;
    default:
      assert(false);
    }
  }
  case SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT: {
    ShovelerWorldEntity* entity =
        shovelerWorldGetEntity(world, clientOp->activateComponent.entityId);
    if (entity == NULL) {
      return false;
    }

    ShovelerComponent* component =
        shovelerWorldEntityGetComponent(entity, clientOp->activateComponent.componentTypeId);
    if (component == NULL) {
      return false;
    }

    ShovelerComponentActivateStatus status = shovelerComponentActivate(component);
    switch (status) {
    case SHOVELER_COMPONENT_ACTIVATE_SUCCESS:
    case SHOVELER_COMPONENT_ACTIVATE_ALREADY_ACTIVE:
      return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
    case SHOVELER_COMPONENT_ACTIVATE_NOT_AUTHORITATIVE:
      return SHOVELER_CLIENT_WORLD_UPDATER_NOT_AUTHORITATIVE;
    case SHOVELER_COMPONENT_ACTIVATE_DEPENDENCIES_INACTIVE:
      return SHOVELER_CLIENT_WORLD_UPDATER_DEPENDENCIES_INACTIVE;
    case SHOVELER_COMPONENT_ACTIVATE_ACTIVATION_FAILURE:
      return SHOVELER_CLIENT_WORLD_UPDATER_ACTIVATION_FAILURE;
    default:
      assert(false);
    }
  }
  case SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT: {
    ShovelerWorldEntity* entity =
        shovelerWorldGetEntity(world, clientOp->deactivateComponent.entityId);
    if (entity == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST;
    }

    ShovelerComponent* component =
        shovelerWorldEntityGetComponent(entity, clientOp->deactivateComponent.componentTypeId);
    if (component == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_DOESNT_EXISTS;
    }

    shovelerComponentDeactivate(component);
    return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
  }
  case SHOVELER_CLIENT_OP_DELEGATE_COMPONENT: {
    ShovelerWorldEntity* entity =
        shovelerWorldGetEntity(world, clientOp->delegateComponent.entityId);
    if (entity == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST;
    }

    shovelerWorldEntityDelegateComponent(entity, clientOp->delegateComponent.componentTypeId);
    return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
  }
  case SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT: {
    ShovelerWorldEntity* entity =
        shovelerWorldGetEntity(world, clientOp->undelegateComponent.entityId);
    if (entity == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST;
    }

    shovelerWorldEntityUndelegateComponent(entity, clientOp->undelegateComponent.componentTypeId);
    return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
  }
  case SHOVELER_CLIENT_OP_REMOVE_COMPONENT: {
    ShovelerWorldEntity* entity = shovelerWorldGetEntity(world, clientOp->removeComponent.entityId);
    if (entity == NULL) {
      return SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST;
    }

    if (!shovelerWorldEntityRemoveComponent(entity, clientOp->removeComponent.componentTypeId)) {
      return SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_DOESNT_EXISTS;
    }
    return SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS;
  }
  default:
    assert(false);
  }
}

const char* shovelerClientWorldUpdaterStatusToString(ShovelerClientWorldUpdaterStatus status) {
  switch (status) {
  case SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS:
    return "success";
  case SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_ALREADY_EXISTS:
    return "entity already exists";
  case SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST:
    return "entity doesn't exist";
  case SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_ALREADY_EXISTS:
    return "component already exists";
  case SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_DOESNT_EXISTS:
    return "component doesn't exist";
  case SHOVELER_CLIENT_WORLD_UPDATER_INVALID_COMPONENT_TYPE:
    return "invalid component type";
  case SHOVELER_CLIENT_WORLD_UPDATER_INVALID_FIELD_TYPE:
    return "invalid component field type";
  case SHOVELER_CLIENT_WORLD_UPDATER_NOT_AUTHORITATIVE:
    return "not authoritative";
  case SHOVELER_CLIENT_WORLD_UPDATER_DEPENDENCIES_INACTIVE:
    return "some dependencies are inactive";
  case SHOVELER_CLIENT_WORLD_UPDATER_ACTIVATION_FAILURE:
    return "activation failure";
  default:
    return "unknown status";
  }
}
