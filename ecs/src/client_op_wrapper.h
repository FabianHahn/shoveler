#ifndef SHOVELER_CLIENT_OP_WRAPPER_H
#define SHOVELER_CLIENT_OP_WRAPPER_H

extern "C" {
#include <shoveler/client_op.h>
};

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Helper type to allow using ShovelerClientOp objects safely from C++.
struct ShovelerClientOpWrapper {
  ShovelerClientOpWrapper(const ShovelerClientOp* input = nullptr) {
    opWithData = shovelerClientOpCreateWithData(input);
  }

  ShovelerClientOpWrapper(const ShovelerClientOpWrapper& other) {
    opWithData = shovelerClientOpCreateWithData(&*other);
  }

  ~ShovelerClientOpWrapper() { shovelerClientOpFreeWithData(opWithData); }

  ShovelerClientOpWrapper& operator=(const ShovelerClientOpWrapper& other) {
    if (&other != this) {
      shovelerClientOpClearWithData(opWithData);
      shovelerClientOpInitWithData(opWithData, &other.opWithData->op);
    }

    return *this;
  }

  const ShovelerClientOp& operator*() const { return opWithData->op; }

  const ShovelerClientOp* operator->() const { return &opWithData->op; }

  ShovelerClientOpWithData* opWithData;
};

inline std::ostream& operator<<(std::ostream& output, const ShovelerClientOpWrapper& clientOp) {
  char* debugOutput = shovelerClientOpDebugPrint(&*clientOp);
  output << debugOutput;
  free(debugOutput);
  return output;
}

MATCHER_P(IsAddEntityOp, entityId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_ADD_ENTITY && call->addEntity.entityId == entityId;
}

MATCHER_P(IsRemoveEntityOp, entityId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_REMOVE_ENTITY && call->removeEntity.entityId == entityId;
}

MATCHER_P2(IsAddComponentOp, entityId, componentTypeId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_ADD_COMPONENT &&
      call->addComponent.entityId == entityId &&
      call->addComponent.componentTypeId == componentTypeId;
}

MATCHER_P3(IsUpdateComponentOp, entityId, componentTypeId, fieldId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_UPDATE_COMPONENT &&
      call->updateComponent.entityId == entityId &&
      call->updateComponent.componentTypeId == componentTypeId &&
      call->updateComponent.fieldId == fieldId;
}

MATCHER_P4(IsUpdateComponentOp, entityId, componentTypeId, fieldId, fieldValue, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_UPDATE_COMPONENT &&
      call->updateComponent.entityId == entityId &&
      call->updateComponent.componentTypeId == componentTypeId &&
      call->updateComponent.fieldId == fieldId &&
      shovelerComponentFieldCompareValue(call->updateComponent.fieldValue, fieldValue);
}

MATCHER_P2(IsActivateComponentOp, entityId, componentTypeId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT &&
      call->activateComponent.entityId == entityId &&
      call->activateComponent.componentTypeId == componentTypeId;
}

MATCHER_P2(IsDeactivateComponentOp, entityId, componentTypeId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT &&
      call->deactivateComponent.entityId == entityId &&
      call->deactivateComponent.componentTypeId == componentTypeId;
}

MATCHER_P2(IsDelegateComponentOp, entityId, componentTypeId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_DELEGATE_COMPONENT &&
      call->delegateComponent.entityId == entityId &&
      call->delegateComponent.componentTypeId == componentTypeId;
}

MATCHER_P2(IsUndelegateComponentOp, entityId, componentTypeId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT &&
      call->undelegateComponent.entityId == entityId &&
      call->undelegateComponent.componentTypeId == componentTypeId;
}

MATCHER_P2(IsRemoveComponentOp, entityId, componentTypeId, "") {
  const ShovelerClientOpWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_OP_REMOVE_COMPONENT &&
      call->removeComponent.entityId == entityId &&
      call->removeComponent.componentTypeId == componentTypeId;
}

#endif
