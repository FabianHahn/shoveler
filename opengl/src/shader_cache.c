#include <stdlib.h> // malloc free

#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/shader.h"
#include "shoveler/shader_cache.h"

static void freeShader(void *shaderPointer);
static void freeList(void *listPointer);

ShovelerShaderCache *shovelerShaderCacheCreate()
{
	ShovelerShaderCache *cache = malloc(sizeof(ShovelerShaderCache));
	cache->shaders = g_hash_table_new_full(shovelerShaderKeyHash, shovelerShaderKeyEqual, NULL, freeShader);
	cache->cameraShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeList);
	cache->lightShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeList);
	cache->modelShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeList);
	cache->materialShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeList);
	cache->userDataShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeList);

	return cache;
}

void shovelerShaderCacheInsert(ShovelerShaderCache *cache, ShovelerShader *shader)
{
	g_hash_table_insert(cache->shaders, &shader->key, shader);

	GQueue *cameraKeys = g_hash_table_lookup(cache->cameraShaderKeys, shader->key.camera);
	if(cameraKeys == NULL) {
		cameraKeys = g_queue_new();
		g_hash_table_insert(cache->cameraShaderKeys, shader->key.camera, cameraKeys);
	}
	g_queue_push_tail(cameraKeys, &shader->key);

	GQueue *lightKeys = g_hash_table_lookup(cache->lightShaderKeys, shader->key.light);
	if(lightKeys == NULL) {
		lightKeys = g_queue_new();
		g_hash_table_insert(cache->lightShaderKeys, shader->key.light, lightKeys);
	}
	g_queue_push_tail(lightKeys, &shader->key);

	GQueue *modelKeys = g_hash_table_lookup(cache->modelShaderKeys, shader->key.model);
	if(modelKeys == NULL) {
		modelKeys = g_queue_new();
		g_hash_table_insert(cache->modelShaderKeys, shader->key.model, modelKeys);
	}
	g_queue_push_tail(modelKeys, &shader->key);

	GQueue *materialKeys = g_hash_table_lookup(cache->materialShaderKeys, shader->key.material);
	if(materialKeys == NULL) {
		materialKeys = g_queue_new();
		g_hash_table_insert(cache->materialShaderKeys, shader->key.material, materialKeys);
	}
	g_queue_push_tail(materialKeys, &shader->key);

	GQueue *userDataKeys = g_hash_table_lookup(cache->userDataShaderKeys, shader->key.userData);
	if(userDataKeys == NULL) {
		userDataKeys = g_queue_new();
		g_hash_table_insert(cache->userDataShaderKeys, shader->key.userData, userDataKeys);
	}
	g_queue_push_tail(userDataKeys, &shader->key);
}

ShovelerShader *shovelerShaderCacheLookup(ShovelerShaderCache *cache, const ShovelerShaderKey *shaderKey)
{
	return g_hash_table_lookup(cache->shaders, shaderKey);
}

void shovelerShaderCacheInvalidateCamera(ShovelerShaderCache *cache, ShovelerCamera *camera)
{
	GQueue *cameraKeys = g_hash_table_lookup(cache->cameraShaderKeys, camera);
	if(cameraKeys != NULL) {
		for(GList *iter = cameraKeys->head; iter != NULL; iter = iter->next) {
			const ShovelerShaderKey *shaderKey = iter->data;
			g_hash_table_remove(cache->shaders, shaderKey);
		}
		g_hash_table_remove(cache->cameraShaderKeys, camera);
	}
}

void shovelerShaderCacheInvalidateLight(ShovelerShaderCache *cache, ShovelerLight *light)
{
	GQueue *lightKeys = g_hash_table_lookup(cache->lightShaderKeys, light);
	if(lightKeys != NULL) {
		for(GList *iter = lightKeys->head; iter != NULL; iter = iter->next) {
			const ShovelerShaderKey *shaderKey = iter->data;
			g_hash_table_remove(cache->shaders, shaderKey);
		}
		g_hash_table_remove(cache->lightShaderKeys, light);
	}
}

void shovelerShaderCacheInvalidateModel(ShovelerShaderCache *cache, ShovelerModel *model)
{
	GQueue *modelKeys = g_hash_table_lookup(cache->modelShaderKeys, model);
	if(modelKeys != NULL) {
		for(GList *iter = modelKeys->head; iter != NULL; iter = iter->next) {
			const ShovelerShaderKey *shaderKey = iter->data;
			g_hash_table_remove(cache->shaders, shaderKey);
		}
		g_hash_table_remove(cache->modelShaderKeys, model);
	}
}

void shovelerShaderCacheInvalidateMaterial(ShovelerShaderCache *cache, ShovelerMaterial *material)
{
	GQueue *materialKeys = g_hash_table_lookup(cache->materialShaderKeys, material);
	if(materialKeys != NULL) {
		for(GList *iter = materialKeys->head; iter != NULL; iter = iter->next) {
			const ShovelerShaderKey *shaderKey = iter->data;
			g_hash_table_remove(cache->shaders, shaderKey);
		}
		g_hash_table_remove(cache->materialShaderKeys, material);
	}
}

void shovelerShaderCacheInvalidateUserData(ShovelerShaderCache *cache, void *userData)
{
	GQueue *userDataKeys = g_hash_table_lookup(cache->userDataShaderKeys, userData);
	if(userDataKeys != NULL) {
		for(GList *iter = userDataKeys->head; iter != NULL; iter = iter->next) {
			const ShovelerShaderKey *shaderKey = iter->data;
			g_hash_table_remove(cache->shaders, shaderKey);
		}
		g_hash_table_remove(cache->userDataShaderKeys, userData);
	}
}

void shovelerShaderCacheFree(ShovelerShaderCache *cache)
{
	g_hash_table_destroy(cache->userDataShaderKeys);
	g_hash_table_destroy(cache->materialShaderKeys);
	g_hash_table_destroy(cache->modelShaderKeys);
	g_hash_table_destroy(cache->lightShaderKeys);
	g_hash_table_destroy(cache->cameraShaderKeys);
	g_hash_table_destroy(cache->shaders);
	free(cache);
}

static void freeShader(void *shaderPointer)
{
	ShovelerShader *shader = shaderPointer;
	shovelerShaderFree(shader);
}

static void freeList(void *listPointer)
{
	GQueue *list = listPointer;
	g_queue_free(list);
}
