#ifndef SHOVELER_CLIENT_SYSTEM_H
#define SHOVELER_CLIENT_SYSTEM_H

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerCollidersStruct ShovelerColliders;
typedef struct ShovelerComponentStruct ShovelerComponent;
typedef struct ShovelerComponentFieldStruct ShovelerComponentField;
typedef struct ShovelerComponentFieldValueStruct ShovelerComponentFieldValue;
typedef struct ShovelerControllerStruct ShovelerController;
typedef struct ShovelerExecutorStruct ShovelerExecutor;
typedef struct ShovelerExecutorCallbackStruct ShovelerExecutorCallback;
typedef struct ShovelerFontsStruct ShovelerFonts;
typedef struct ShovelerGameStruct ShovelerGame;
typedef struct ShovelerInputStruct ShovelerInput;
typedef struct ShovelerInputKeyCallbackStruct ShovelerInputKeyCallback;
typedef struct ShovelerRenderStateStruct ShovelerRenderState;
typedef struct ShovelerSchemaStruct ShovelerSchema;
typedef struct ShovelerSceneStruct ShovelerScene;
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache;
typedef struct ShovelerSystemStruct ShovelerSystem;
typedef struct ShovelerWorldCallbacksStruct ShovelerWorldCallbacks;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef struct ShovelerClientSystemStruct {
  ShovelerSystem* system;
  ShovelerSchema* schema;
  ShovelerWorld* world;
  ShovelerExecutor* executor;
  ShovelerInput* input;
  ShovelerColliders* colliders;
  ShovelerFonts* fonts;
  ShovelerController* controller;
  ShovelerShaderCache* shaderCache;
  ShovelerScene* scene;
  ShovelerRenderState* renderState;
  ShovelerInputKeyCallback* keyCallback;
  ShovelerExecutorCallback* updateWorldCountersExecutorCallback;
  struct {
    unsigned int numEntities;
    unsigned int numComponents;
    unsigned int numComponentDependencies;
    unsigned int numActiveComponents;
  } lastWorldCounters;
} ShovelerClientSystem;

ShovelerClientSystem* shovelerClientSystemCreate(
    ShovelerGame* game, ShovelerWorldCallbacks* worldCallbacks);
void shovelerClientSystemFree(ShovelerClientSystem* clientSystem);

#endif
