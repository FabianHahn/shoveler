#ifndef SHOVELER_SERVER_OP_H
#define SHOVELER_SERVER_OP_H

#include <glib.h>
#include <shoveler/component_field.h>
#include <stdbool.h>

typedef struct ShovelerComponentTypeIndexerStruct ShovelerComponentTypeIndexer;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef enum {
  SHOVELER_SERVER_OP_NOOP,
  SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST,
  SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST,
  SHOVELER_SERVER_OP_UPDATE_COMPONENT,
} ShovelerServerOpType;

typedef struct ShovelerServerOpNoopStruct {
} ShovelerServerOpNoop;

typedef struct ShovelerServerOpAddEntityInterestStruct {
  long long int entityId;
} ShovelerServerOpAddEntityInterest;

typedef struct ShovelerServerOpRemoveEntityInterestStruct {
  long long int entityId;
} ShovelerServerOpRemoveEntityInterest;

typedef struct ShovelerServerOpUpdateComponentStruct {
  long long int entityId;
  const char* componentTypeId;
  int fieldId;
  const ShovelerComponentFieldValue* fieldValue;
} ShovelerServerOpUpdateComponent;

typedef struct ShovelerServerOpStruct {
  ShovelerServerOpType type;
  union {
    ShovelerServerOpNoop noop;
    ShovelerServerOpAddEntityInterest addEntityInterest;
    ShovelerServerOpRemoveEntityInterest removeEntityInterest;
    ShovelerServerOpUpdateComponent updateComponent;
  };
} ShovelerServerOp;

typedef struct ShovelerServerOpWithDataStruct {
  ShovelerServerOp op;
  ShovelerComponentFieldValue fieldValue;
} ShovelerServerOpWithData;

ShovelerServerOp shovelerServerOp();
ShovelerServerOpWithData* shovelerServerOpCreateWithData(const ShovelerServerOp* inputServerOp);
void shovelerServerOpInitWithData(
    ShovelerServerOpWithData* serverOp, const ShovelerServerOp* inputServerOp);
void shovelerServerOpClear(ShovelerServerOp* serverOp);
void shovelerServerOpClearWithData(ShovelerServerOpWithData* serverOp);
void shovelerServerOpFreeWithData(ShovelerServerOpWithData* serverOp);
bool shovelerServerOpEquals(const ShovelerServerOp* serverOp1, const ShovelerServerOp* serverOp2);
/** Appends the serialized representation of the client op to the passed string. */
bool shovelerServerOpSerialize(
    const ShovelerServerOp* serverOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    GString* output);
bool shovelerServerOpDeserialize(
    ShovelerServerOpWithData* serverOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    const unsigned char* buffer,
    int bufferSize,
    int* readIndex);
char* shovelerServerOpDebugPrint(const ShovelerServerOp* serverOp);

#endif