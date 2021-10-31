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
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef void(ShovelerClientSystemUpdateAuthoritativeComponentFunction)(
    ShovelerClientSystem* clientSystem,
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* userData);

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
  ShovelerClientSystemUpdateAuthoritativeComponentFunction* updateAuthoritativeComponent;
  void* updateAuthoritativeComponentUserData;
  ShovelerInputKeyCallback* keyCallback;
  ShovelerExecutorCallback* updateWorldCountersExecutorCallback;
  struct {
    unsigned int numEntities;
    unsigned int numComponents;
    unsigned int numComponentDependencies;
  } lastWorldCounters;
} ShovelerClientSystem;

ShovelerClientSystem* shovelerClientSystemCreate(
    ShovelerGame* game,
    ShovelerClientSystemUpdateAuthoritativeComponentFunction* updateAuthoritativeComponent,
    void* updateAuthoritativeComponentUserData);
void shovelerClientSystemFree(ShovelerClientSystem* clientSystem);

#endif
