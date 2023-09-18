#ifndef SHOVELER_CLIENT_OP_H
#define SHOVELER_CLIENT_OP_H

#include <glib.h>
#include <shoveler/component_field.h>
#include <stdbool.h>

typedef struct ShovelerComponentTypeIndexerStruct ShovelerComponentTypeIndexer;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef enum {
  SHOVELER_CLIENT_OP_NOOP,
  SHOVELER_CLIENT_OP_ADD_ENTITY,
  SHOVELER_CLIENT_OP_REMOVE_ENTITY,
  SHOVELER_CLIENT_OP_ADD_COMPONENT,
  SHOVELER_CLIENT_OP_UPDATE_COMPONENT,
  SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT,
  SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT,
  SHOVELER_CLIENT_OP_DELEGATE_COMPONENT,
  SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT,
  SHOVELER_CLIENT_OP_REMOVE_COMPONENT,
} ShovelerClientOpType;

typedef struct ShovelerClientOpAddEntityStruct {
  long long int entityId;
} ShovelerClientOpAddEntity;

typedef struct ShovelerClientOpRemoveEntityStruct {
  long long int entityId;
} ShovelerClientOpRemoveEntity;

typedef struct ShovelerClientOpAddComponentStruct {
  long long int entityId;
  const char* componentTypeId;
} ShovelerClientOpAddComponent;

typedef struct ShovelerClientOpUpdateComponentStruct {
  long long int entityId;
  const char* componentTypeId;
  int fieldId;
  const ShovelerComponentFieldValue* fieldValue;
} ShovelerClientOpUpdateComponent;

typedef struct ShovelerClientOpActivateComponentStruct {
  long long int entityId;
  const char* componentTypeId;
} ShovelerClientOpActivateComponent;

typedef struct ShovelerClientOpDeactivateComponentStruct {
  long long int entityId;
  const char* componentTypeId;
} ShovelerClientOpDeactivateComponent;

typedef struct ShovelerClientOpDelegateComponentStruct {
  long long int entityId;
  const char* componentTypeId;
} ShovelerClientOpDelegateComponent;

typedef struct ShovelerClientOpUndelegateComponentStruct {
  long long int entityId;
  const char* componentTypeId;
} ShovelerClientOpUndelegateComponent;

typedef struct ShovelerClientOpRemoveComponentStruct {
  long long int entityId;
  const char* componentTypeId;
} ShovelerClientOpRemoveComponent;

typedef struct ShovelerClientOpStruct {
  ShovelerClientOpType type;
  union {
    ShovelerClientOpAddEntity addEntity;
    ShovelerClientOpRemoveEntity removeEntity;
    ShovelerClientOpAddComponent addComponent;
    ShovelerClientOpUpdateComponent updateComponent;
    ShovelerClientOpActivateComponent activateComponent;
    ShovelerClientOpDeactivateComponent deactivateComponent;
    ShovelerClientOpDelegateComponent delegateComponent;
    ShovelerClientOpUndelegateComponent undelegateComponent;
    ShovelerClientOpRemoveComponent removeComponent;
  };
} ShovelerClientOp;

typedef struct ShovelerClientOpWithDataStruct {
  ShovelerClientOp op;
  ShovelerComponentFieldValue fieldValue;
} ShovelerClientOpWithData;

ShovelerClientOp shovelerClientOp();
/** inputClientOp can be NULL */
ShovelerClientOpWithData* shovelerClientOpCreateWithData(const ShovelerClientOp* inputClientOp);
void shovelerClientOpInitWithData(
    ShovelerClientOpWithData* clientOp, const ShovelerClientOp* inputClientOp);
void shovelerClientOpClear(ShovelerClientOp* clientOp);
void shovelerClientOpClearWithData(ShovelerClientOpWithData* clientOp);
void shovelerClientOpFreeWithData(ShovelerClientOpWithData* clientOp);
bool shovelerClientOpEquals(const ShovelerClientOp* clientOp1, const ShovelerClientOp* clientOp2);
/** Appends the serialized representation of the client op to the passed string. */
bool shovelerClientOpSerialize(
    const ShovelerClientOp* clientOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    GString* output);
bool shovelerClientOpDeserialize(
    ShovelerClientOpWithData* clientOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    const unsigned char* buffer,
    int bufferSize,
    int* readIndex);
char* shovelerClientOpDebugPrint(const ShovelerClientOp* clientOp);

#endif
