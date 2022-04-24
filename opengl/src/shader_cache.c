#include "shoveler/shader_cache.h"

#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/shader.h"

static void freeShader(void* shaderPointer);
static void freeSet(void* setPointer);

ShovelerShaderCache* shovelerShaderCacheCreate() {
  return shovelerShaderCacheCreateWithCustomFree(freeShader);
}

ShovelerShaderCache* shovelerShaderCacheCreateWithCustomFree(
    ShovelerShaderCacheFreeShaderFunction* freeShader) {
  ShovelerShaderCache* cache = malloc(sizeof(ShovelerShaderCache));
  cache->shaders =
      g_hash_table_new_full(shovelerShaderKeyHash, shovelerShaderKeyEqual, NULL, freeShader);
  cache->sceneShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeSet);
  cache->cameraShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeSet);
  cache->lightShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeSet);
  cache->modelShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeSet);
  cache->materialShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeSet);
  cache->userDataShaderKeys = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeSet);

  return cache;
}

void shovelerShaderCacheInsert(ShovelerShaderCache* cache, ShovelerShader* shader) {
  g_hash_table_insert(cache->shaders, &shader->key, shader);

  GHashTable* sceneKeys = g_hash_table_lookup(cache->sceneShaderKeys, shader->key.scene);
  if (sceneKeys == NULL) {
    sceneKeys = g_hash_table_new(shovelerShaderKeyHash, shovelerShaderKeyEqual);
    g_hash_table_insert(cache->sceneShaderKeys, shader->key.scene, sceneKeys);
  }
  g_hash_table_add(sceneKeys, &shader->key);

  GHashTable* cameraKeys = g_hash_table_lookup(cache->cameraShaderKeys, shader->key.camera);
  if (cameraKeys == NULL) {
    cameraKeys = g_hash_table_new(shovelerShaderKeyHash, shovelerShaderKeyEqual);
    g_hash_table_insert(cache->cameraShaderKeys, shader->key.camera, cameraKeys);
  }
  g_hash_table_add(cameraKeys, &shader->key);

  GHashTable* lightKeys = g_hash_table_lookup(cache->lightShaderKeys, shader->key.light);
  if (lightKeys == NULL) {
    lightKeys = g_hash_table_new(shovelerShaderKeyHash, shovelerShaderKeyEqual);
    g_hash_table_insert(cache->lightShaderKeys, shader->key.light, lightKeys);
  }
  g_hash_table_add(lightKeys, &shader->key);

  GHashTable* modelKeys = g_hash_table_lookup(cache->modelShaderKeys, shader->key.model);
  if (modelKeys == NULL) {
    modelKeys = g_hash_table_new(shovelerShaderKeyHash, shovelerShaderKeyEqual);
    g_hash_table_insert(cache->modelShaderKeys, shader->key.model, modelKeys);
  }
  g_hash_table_add(modelKeys, &shader->key);

  GHashTable* materialKeys = g_hash_table_lookup(cache->materialShaderKeys, shader->key.material);
  if (materialKeys == NULL) {
    materialKeys = g_hash_table_new(shovelerShaderKeyHash, shovelerShaderKeyEqual);
    g_hash_table_insert(cache->materialShaderKeys, shader->key.material, materialKeys);
  }
  g_hash_table_add(materialKeys, &shader->key);

  GHashTable* userDataKeys = g_hash_table_lookup(cache->userDataShaderKeys, shader->key.userData);
  if (userDataKeys == NULL) {
    userDataKeys = g_hash_table_new(shovelerShaderKeyHash, shovelerShaderKeyEqual);
    g_hash_table_insert(cache->userDataShaderKeys, shader->key.userData, userDataKeys);
  }
  g_hash_table_add(userDataKeys, &shader->key);
}

bool shovelerShaderCacheRemove(ShovelerShaderCache* cache, const ShovelerShaderKey* shaderKey) {
  ShovelerShader* shader = g_hash_table_lookup(cache->shaders, shaderKey);
  if (shader == NULL) {
    return false;
  }

  GHashTable* sceneKeys = g_hash_table_lookup(cache->sceneShaderKeys, shaderKey->scene);
  assert(sceneKeys != NULL);
  bool sceneKeyRemoved = g_hash_table_remove(sceneKeys, shaderKey);
  assert(sceneKeyRemoved);

  GHashTable* cameraKeys = g_hash_table_lookup(cache->cameraShaderKeys, shaderKey->camera);
  assert(cameraKeys != NULL);
  bool cameraKeyRemoved = g_hash_table_remove(cameraKeys, shaderKey);
  assert(cameraKeyRemoved);

  GHashTable* lightKeys = g_hash_table_lookup(cache->lightShaderKeys, shaderKey->light);
  assert(lightKeys != NULL);
  bool lightKeyRemoved = g_hash_table_remove(lightKeys, shaderKey);
  assert(lightKeyRemoved);

  GHashTable* modelKeys = g_hash_table_lookup(cache->modelShaderKeys, shaderKey->model);
  assert(modelKeys != NULL);
  bool modelKeyRemoved = g_hash_table_remove(modelKeys, shaderKey);
  assert(modelKeyRemoved);

  GHashTable* materialKeys = g_hash_table_lookup(cache->materialShaderKeys, shaderKey->material);
  assert(materialKeys != NULL);
  bool materialKeyRemoved = g_hash_table_remove(materialKeys, shaderKey);
  assert(materialKeyRemoved);

  GHashTable* userDataKeys = g_hash_table_lookup(cache->userDataShaderKeys, shaderKey->userData);
  assert(userDataKeys != NULL);
  bool userDataKeyRemoved = g_hash_table_remove(userDataKeys, shaderKey);
  assert(userDataKeyRemoved);

  // make sure we remove the shader itself last, because the key tables use the shader key from the
  // stored shader
  g_hash_table_remove(cache->shaders, shaderKey);
  return true;
}

ShovelerShader* shovelerShaderCacheLookup(
    ShovelerShaderCache* cache, const ShovelerShaderKey* shaderKey) {
  return g_hash_table_lookup(cache->shaders, shaderKey);
}

void shovelerShaderCacheInvalidateScene(
    ShovelerShaderCache* cache, struct ShovelerSceneStruct* scene) {
  GHashTable* sceneKeys = g_hash_table_lookup(cache->sceneShaderKeys, scene);
  if (sceneKeys != NULL) {
    // get keys list because the keys set will be modified while we iterate over the list
    GList* sceneKeysList = g_hash_table_get_keys(sceneKeys);
    for (GList* iter = sceneKeysList; iter != NULL; iter = iter->next) {
      const ShovelerShaderKey* shaderKey = iter->data;
      bool removed = shovelerShaderCacheRemove(cache, shaderKey);
      assert(removed);
    }
    g_list_free(sceneKeysList);
    g_hash_table_remove(cache->sceneShaderKeys, scene);
  }
}

void shovelerShaderCacheInvalidateCamera(ShovelerShaderCache* cache, ShovelerCamera* camera) {
  GHashTable* cameraKeys = g_hash_table_lookup(cache->cameraShaderKeys, camera);
  if (cameraKeys != NULL) {
    // get keys list because the keys set will be modified while we iterate over the list
    GList* cameraKeysList = g_hash_table_get_keys(cameraKeys);
    for (GList* iter = cameraKeysList; iter != NULL; iter = iter->next) {
      const ShovelerShaderKey* shaderKey = iter->data;
      bool removed = shovelerShaderCacheRemove(cache, shaderKey);
      assert(removed);
    }
    g_list_free(cameraKeysList);
    g_hash_table_remove(cache->cameraShaderKeys, camera);
  }
}

void shovelerShaderCacheInvalidateLight(ShovelerShaderCache* cache, ShovelerLight* light) {
  GHashTable* lightKeys = g_hash_table_lookup(cache->lightShaderKeys, light);
  if (lightKeys != NULL) {
    // get keys list because the keys set will be modified while we iterate over the list
    GList* lightKeysList = g_hash_table_get_keys(lightKeys);
    for (GList* iter = lightKeysList; iter != NULL; iter = iter->next) {
      const ShovelerShaderKey* shaderKey = iter->data;
      bool removed = shovelerShaderCacheRemove(cache, shaderKey);
      assert(removed);
    }
    g_list_free(lightKeysList);
    g_hash_table_remove(cache->lightShaderKeys, light);
  }
}

void shovelerShaderCacheInvalidateModel(ShovelerShaderCache* cache, ShovelerModel* model) {
  GHashTable* modelKeys = g_hash_table_lookup(cache->modelShaderKeys, model);
  if (modelKeys != NULL) {
    // get keys list because the keys set will be modified while we iterate over the list
    GList* modelKeysList = g_hash_table_get_keys(modelKeys);
    for (GList* iter = modelKeysList; iter != NULL; iter = iter->next) {
      const ShovelerShaderKey* shaderKey = iter->data;
      bool removed = shovelerShaderCacheRemove(cache, shaderKey);
      assert(removed);
    }
    g_list_free(modelKeysList);
    g_hash_table_remove(cache->modelShaderKeys, model);
  }
}

void shovelerShaderCacheInvalidateMaterial(ShovelerShaderCache* cache, ShovelerMaterial* material) {
  GHashTable* materialKeys = g_hash_table_lookup(cache->materialShaderKeys, material);
  if (materialKeys != NULL) {
    // get keys list because the keys set will be modified while we iterate over the list
    GList* materialKeysList = g_hash_table_get_keys(materialKeys);
    for (GList* iter = materialKeysList; iter != NULL; iter = iter->next) {
      const ShovelerShaderKey* shaderKey = iter->data;
      bool removed = shovelerShaderCacheRemove(cache, shaderKey);
      assert(removed);
    }
    g_list_free(materialKeysList);
    g_hash_table_remove(cache->materialShaderKeys, material);
  }
}

void shovelerShaderCacheInvalidateUserData(ShovelerShaderCache* cache, void* userData) {
  GHashTable* userDataKeys = g_hash_table_lookup(cache->userDataShaderKeys, userData);
  if (userDataKeys != NULL) {
    // get keys list because the keys set will be modified while we iterate over the list
    GList* userDataKeysList = g_hash_table_get_keys(userDataKeys);
    for (GList* iter = userDataKeysList; iter != NULL; iter = iter->next) {
      const ShovelerShaderKey* shaderKey = iter->data;
      bool removed = shovelerShaderCacheRemove(cache, shaderKey);
      assert(removed);
    }
    g_list_free(userDataKeysList);
    g_hash_table_remove(cache->userDataShaderKeys, userData);
  }
}

void shovelerShaderCacheFree(ShovelerShaderCache* cache) {
  g_hash_table_destroy(cache->userDataShaderKeys);
  g_hash_table_destroy(cache->materialShaderKeys);
  g_hash_table_destroy(cache->modelShaderKeys);
  g_hash_table_destroy(cache->lightShaderKeys);
  g_hash_table_destroy(cache->cameraShaderKeys);
  g_hash_table_destroy(cache->sceneShaderKeys);
  g_hash_table_destroy(cache->shaders);
  free(cache);
}

static void freeShader(void* shaderPointer) {
  ShovelerShader* shader = shaderPointer;
  shovelerShaderFree(shader);
}

static void freeSet(void* setPointer) {
  GHashTable* set = setPointer;
  g_hash_table_destroy(set);
}
