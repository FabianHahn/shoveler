#ifndef SHOVELER_SERVER_OP_WRAPPER_H
#define SHOVELER_SERVER_OP_WRAPPER_H

extern "C" {
#include <shoveler/server_op.h>
};

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Helper type to allow using ShovelerServerOp objects safely from C++.
struct ShovelerServerOpWrapper {
  ShovelerServerOpWrapper(const ShovelerServerOp* input) {
    opWithData = shovelerServerOpCreateWithData(input);
  }

  ShovelerServerOpWrapper(const ShovelerServerOpWrapper& other) {
    opWithData = shovelerServerOpCreateWithData(&*other);
  }

  ~ShovelerServerOpWrapper() { shovelerServerOpFreeWithData(opWithData); }

  ShovelerServerOpWrapper& operator=(const ShovelerServerOpWrapper& other) {
    if (&other != this) {
      shovelerServerOpClearWithData(opWithData);
      shovelerServerOpInitWithData(opWithData, &other.opWithData->op);
    }

    return *this;
  }

  const ShovelerServerOp& operator*() const { return opWithData->op; }

  const ShovelerServerOp* operator->() const { return &opWithData->op; }

  ShovelerServerOpWithData* opWithData;
};

inline std::ostream& operator<<(std::ostream& output, const ShovelerServerOpWrapper& serverOp) {
  char* debugOutput = shovelerServerOpDebugPrint(&*serverOp);
  output << debugOutput;
  free(debugOutput);
  return output;
}

MATCHER_P(IsAddEntityInterestOp, entityId, "") {
  const ShovelerServerOpWrapper& call = arg;
  return call->type == SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST &&
      call->addEntityInterest.entityId == entityId;
}

#endif
