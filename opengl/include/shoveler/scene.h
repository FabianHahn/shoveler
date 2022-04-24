#ifndef SHOVELER_SCENE_H
#define SHOVELER_SCENE_H

#include <glib.h>
#include <shoveler/render_state.h>
#include <shoveler/types.h>
#include <stdbool.h> // bool

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerFramebufferStruct ShovelerFramebuffer; // forward declaration: framebuffer.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerShaderStruct ShovelerShader; // forward declaration: shader.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerUniformMapStruct ShovelerUniformMap; // forward declaration: uniform_map.h

typedef struct ShovelerSceneStruct {
  ShovelerShaderCache* shaderCache;
  ShovelerUniformMap* uniforms;
  ShovelerMaterial* depthMaterial;
  /* private */ bool debugMode;
  /* private */ ShovelerVector2 activeFramebufferSize;
  GHashTable* lights;
  GHashTable* models;
} ShovelerScene;

typedef struct {
  ShovelerMaterial* overrideMaterial;
  bool emitters;
  bool screenspace;
  bool onlyShadowCasters;
  ShovelerRenderState renderState;
} ShovelerSceneRenderPassOptions;

ShovelerScene* shovelerSceneCreate(ShovelerShaderCache* shaderCache);
void shovelerSceneToggleDebugMode(ShovelerScene* scene);
bool shovelerSceneAddLight(ShovelerScene* scene, ShovelerLight* light);
bool shovelerSceneRemoveLight(ShovelerScene* scene, ShovelerLight* light);
bool shovelerSceneAddModel(ShovelerScene* scene, ShovelerModel* model);
bool shovelerSceneRemoveModel(ShovelerScene* scene, ShovelerModel* model);
int shovelerSceneRenderPass(
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerSceneRenderPassOptions options,
    ShovelerRenderState* renderState);
int shovelerSceneRenderFrame(
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerFramebuffer* framebuffer,
    ShovelerRenderState* renderState);
/** Generates a shader, where shaders for calls to this with the same arguments might be cached. */
ShovelerShader* shovelerSceneGenerateShader(
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerModel* model,
    ShovelerMaterial* material,
    void* userData);
void shovelerSceneFree(ShovelerScene* scene);

#endif
