#ifndef SHOVELER_SHADER_CACHE_H
#define SHOVELER_SHADER_CACHE_H

#include <glib.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerModelStruct; // forward declaration: model.h
struct ShovelerSceneStruct; // forward declaration: scene.h
struct ShovelerShaderStruct; // forward declaration: shader.h
struct ShovelerShaderKeyStruct; // forward declaration: shader.h

typedef struct ShovelerShaderCacheStruct {
	/** map from (ShovelerShaderKey *) to (ShovelerShader *) */
	GHashTable *shaders;
	/** map from (ShovelerCamera *) to list of (ShovelerShaderKey *) */
	GHashTable *cameraShaderKeys;
	/** map from (ShovelerLight *) to list of (ShovelerShaderKey *) */
	GHashTable *lightShaderKeys;
	/** map from (ShovelerModel *) to list of (ShovelerShaderKey *) */
	GHashTable *modelShaderKeys;
	/** map from (ShovelerMaterial *) to list of (ShovelerShaderKey *) */
	GHashTable *materialShaderKeys;
	/** map from (void *) to list of (ShovelerShaderKey *) */
	GHashTable *userDataShaderKeys;
} ShovelerShaderCache;

ShovelerShaderCache *shovelerShaderCacheCreate();
/** Insert a shader into the cache, transferring ownership over it to the cache. */
void shovelerShaderCacheInsert(ShovelerShaderCache *cache, struct ShovelerShaderStruct *shader);
struct ShovelerShaderStruct *shovelerShaderCacheLookup(ShovelerShaderCache *cache, const struct ShovelerShaderKeyStruct *shaderKey);
void shovelerShaderCacheInvalidateCamera(ShovelerShaderCache *cache, struct ShovelerCameraStruct *camera);
void shovelerShaderCacheInvalidateLight(ShovelerShaderCache *cache, struct ShovelerLightStruct *light);
void shovelerShaderCacheInvalidateModel(ShovelerShaderCache *cache, struct ShovelerModelStruct *model);
void shovelerShaderCacheInvalidateMaterial(ShovelerShaderCache *cache, struct ShovelerMaterialStruct *material);
void shovelerShaderCacheInvalidateUserData(ShovelerShaderCache *cache, void *userData);
void shovelerShaderCacheFree(ShovelerShaderCache *cache);

#endif
