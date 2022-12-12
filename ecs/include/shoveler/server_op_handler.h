#ifndef SHOVELER_SERVER_OP_HANDLER_H
#define SHOVELER_SERVER_OP_HANDLER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct ShovelerClientOpEmitterStruct ShovelerClientOpEmitter;
typedef struct ShovelerClientPropertyManagerStruct ShovelerClientPropertyManager;
typedef struct ShovelerServerOpHandlerStruct ShovelerServerOpHandler;
typedef struct ShovelerServerOpStruct ShovelerServerOp;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef struct ShovelerServerOpHandlerAdapterStruct {
  bool (*hasAuthority)(
      ShovelerServerOpHandler* serverOpHandler,
      int64_t clientId,
      long long int entityId,
      const char* componentTypeId,
      void* userData);

  void* userData;
} ShovelerServerOpHandlerAdapter;

typedef struct ShovelerServerOpHandlerStruct {
  ShovelerClientPropertyManager* clientPropertyManager;
  ShovelerWorld* world;
  ShovelerClientOpEmitter* clientOpEmitter;
  ShovelerServerOpHandlerAdapter* adapter;
} ShovelerServerOpHandler;

/** Caller retains ownership over passed objects. */
ShovelerServerOpHandler shovelerServerOpHandler(
    ShovelerClientPropertyManager* clientPropertyManager,
    ShovelerWorld* world,
    ShovelerClientOpEmitter* clientOpEmitter,
    ShovelerServerOpHandlerAdapter* adapter);
bool shovelerServerOpHandlerHandle(
    ShovelerServerOpHandler* serverOpHandler, int64_t clientId, const ShovelerServerOp* serverOp);

#endif