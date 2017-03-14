#include <stdlib.h> // malloc, free

#include "log.h"
#include "scene.h"
#include "shader.h"

typedef struct {
	GHashTable *materialShaderCache;
} ModelShaderCache;

typedef struct {
	GHashTable *shaders;
} MaterialShaderCache;

static ShovelerShader *generateShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerModel *model, ShovelerMaterial *material);
static ShovelerShader *getCachedShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerModel *model, ShovelerMaterial *material);
static ModelShaderCache *createModelShaderCache();
static MaterialShaderCache *createMaterialShaderCache();
static void freeModelShaderCache(void *cachePointer);
static void freeMaterialShaderCache(void *cachePointer);
static void freeShader(void *shaderPointer);

ShovelerScene *shovelerSceneCreate()
{
	ShovelerScene *scene = malloc(sizeof(ShovelerScene));
	scene->light = NULL;
	scene->uniforms = shovelerUniformMapCreate();
	scene->models = g_queue_new();
	scene->modelShaderCache = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeModelShaderCache);
	return scene;
}

void shovelerSceneAddLight(ShovelerScene *scene, ShovelerLight *light)
{
	shovelerLightFree(scene->light);
	scene->light = light;
}

void shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model)
{
	g_queue_push_tail(scene->models, model);
}

int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, ShovelerMaterial *overrideMaterial, bool onlyShadowCasters)
{
	int rendered = 0;
	for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
		ShovelerModel *model = iter->data;

		if(!model->visible) {
			continue;
		}

		if(onlyShadowCasters && !model->castsShadow) {
			continue;
		}

		ShovelerShader *shader = getCachedShader(scene, camera, model, overrideMaterial == NULL ? model->material : overrideMaterial);

		if(!shovelerShaderUse(shader)) {
			shovelerLogWarning("Failed to use shader for model %p and camera %p, hiding model.", model, camera);
			model->visible = false;
		}

		if(!shovelerModelRender(model)) {
			shovelerLogWarning("Failed to render model %p with camera %p, hiding model.", model, camera);
			model->visible = false;
			continue;
		}

		rendered++;
	}
	return rendered;
}

int shovelerSceneRenderFrame(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	int rendered = 0;

	if(scene->light != NULL) {
		rendered += shovelerLightRender(scene->light, scene);
	}

	shovelerFramebufferUse(framebuffer);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rendered += shovelerSceneRenderPass(scene, camera, NULL, false);

	return rendered;
}

void shovelerSceneFree(ShovelerScene *scene)
{
	shovelerLightFree(scene->light);
	g_hash_table_destroy(scene->modelShaderCache);
	for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
		shovelerModelFree(iter->data);
	}
	g_queue_free(scene->models);
	shovelerUniformMapFree(scene->uniforms);
	free(scene);
}

static ShovelerShader *generateShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerModel *model, ShovelerMaterial *material)
{
	ShovelerShader *shader = shovelerShaderCreate(material);

	int materialAttached = shovelerShaderAttachUniforms(shader, material->uniforms);
	int modelAttached = shovelerShaderAttachUniforms(shader, model->uniforms);
	int cameraAttached = shovelerShaderAttachUniforms(shader, camera->uniforms);
	int sceneAttached = shovelerShaderAttachUniforms(shader, scene->uniforms);

	shovelerLogInfo("Generated shader for scene %p, camera %p, model %p and material %p with shader program %d (uniforms: %d material, %d model, %d camera, %d scene).", scene, camera, model, material, material->program, materialAttached, modelAttached, cameraAttached, sceneAttached);
	return shader;
}

static ShovelerShader *getCachedShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerModel *model, ShovelerMaterial *material)
{
	ModelShaderCache *modelShaderCache = g_hash_table_lookup(scene->modelShaderCache, camera);
	if(modelShaderCache == NULL) {
		modelShaderCache = createModelShaderCache();
		g_hash_table_insert(scene->modelShaderCache, camera, modelShaderCache);
	}

	MaterialShaderCache *materialShaderCache = g_hash_table_lookup(modelShaderCache->materialShaderCache, model);
	if(materialShaderCache == NULL) {
		materialShaderCache = createMaterialShaderCache();
		g_hash_table_insert(modelShaderCache->materialShaderCache, model, materialShaderCache);
	}

	ShovelerShader *shader = g_hash_table_lookup(materialShaderCache->shaders, material);
	if(shader == NULL) {
		shader = generateShader(scene, camera, model, material);
		g_hash_table_insert(materialShaderCache->shaders, material, shader);
	}

	return shader;
}

static ModelShaderCache *createModelShaderCache()
{
	ModelShaderCache *cache = malloc(sizeof(ModelShaderCache));
	cache->materialShaderCache = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeMaterialShaderCache);
	return cache;
}

static MaterialShaderCache *createMaterialShaderCache()
{
	MaterialShaderCache *cache = malloc(sizeof(MaterialShaderCache));
	cache->shaders = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeShader);
	return cache;
}

static void freeModelShaderCache(void *cachePointer)
{
	free(cachePointer);
}

static void freeMaterialShaderCache(void *cachePointer)
{
	free(cachePointer);
}

static void freeShader(void *shaderPointer)
{
	shovelerShaderFree(shaderPointer);
}
