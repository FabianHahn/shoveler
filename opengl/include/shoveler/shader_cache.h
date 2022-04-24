#ifndef SHOVELER_SHADER_CACHE_H
#define SHOVELER_SHADER_CACHE_H

#include <glib.h>
#include <stdbool.h>

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: scene.h
typedef struct ShovelerShaderStruct ShovelerShader; // forward declaration: shader.h
typedef struct ShovelerShaderKeyStruct ShovelerShaderKey; // forward declaration: shader.h

typedef struct ShovelerShaderCacheStruct {
  /** map from (ShovelerShaderKey *) to (ShovelerShader *) */
  GHashTable* shaders;
  /** map from (ShovelerScene *) to set of (ShovelerShaderKey *) */
  GHashTable* sceneShaderKeys;
  /** map from (ShovelerCamera *) to set of (ShovelerShaderKey *) */
  GHashTable* cameraShaderKeys;
  /** map from (ShovelerLight *) to set of (ShovelerShaderKey *) */
  GHashTable* lightShaderKeys;
  /** map from (ShovelerModel *) to set of (ShovelerShaderKey *) */
  GHashTable* modelShaderKeys;
  /** map from (ShovelerMaterial *) to set of (ShovelerShaderKey *) */
  GHashTable* materialShaderKeys;
  /** map from (void *) to set of (ShovelerShaderKey *) */
  GHashTable* userDataShaderKeys;
} ShovelerShaderCache;

typedef void(ShovelerShaderCacheFreeShaderFunction)(void* shaderPointer);

ShovelerShaderCache* shovelerShaderCacheCreate();
ShovelerShaderCache* shovelerShaderCacheCreateWithCustomFree(
    ShovelerShaderCacheFreeShaderFunction* freeShader);
/** Insert a shader into the cache, transferring ownership over it to the cache. */
void shovelerShaderCacheInsert(ShovelerShaderCache* cache, ShovelerShader* shader);
bool shovelerShaderCacheRemove(ShovelerShaderCache* cache, const ShovelerShaderKey* shaderKey);
struct ShovelerShaderStruct* shovelerShaderCacheLookup(
    ShovelerShaderCache* cache, const ShovelerShaderKey* shaderKey);
void shovelerShaderCacheInvalidateScene(ShovelerShaderCache* cache, ShovelerScene* scene);
void shovelerShaderCacheInvalidateCamera(ShovelerShaderCache* cache, ShovelerCamera* camera);
void shovelerShaderCacheInvalidateLight(ShovelerShaderCache* cache, ShovelerLight* light);
void shovelerShaderCacheInvalidateModel(ShovelerShaderCache* cache, ShovelerModel* model);
void shovelerShaderCacheInvalidateMaterial(ShovelerShaderCache* cache, ShovelerMaterial* material);
void shovelerShaderCacheInvalidateUserData(ShovelerShaderCache* cache, void* userData);
void shovelerShaderCacheFree(ShovelerShaderCache* cache);

#endif
