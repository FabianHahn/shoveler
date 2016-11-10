#include <stdlib.h> // malloc, free

#include "material/depth.h"
#include "log.h"
#include "scene.h"
#include "shader.h"

typedef struct {
	GHashTable *materialShaderCache;
} ModelShaderCache;

typedef struct {
	GHashTable *shaders;
} MaterialShaderCache;

static ShovelerShader *generateShader(ShovelerCamera *camera, ShovelerModel *model, ShovelerMaterial *material);
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
	scene->depthMaterial = shovelerMaterialDepthCreate();
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

int shovelerSceneRender(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	int rendered = 0;

	if(scene->light != NULL) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		shovelerFramebufferUse(scene->light->framebuffer);

		for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
			ShovelerModel *model = iter->data;

			if(!model->visible) {
				continue;
			}

			ShovelerShader *depthShader = getCachedShader(scene, scene->light->camera, model, scene->depthMaterial);

			if(!shovelerShaderUse(depthShader)) {
				shovelerLogWarning("Failed to use depth shader for model %p and light %p, hiding model.", model, scene->light->camera);
				model->visible = false;
			}

			if(!shovelerModelRender(model)) {
				shovelerLogWarning("Failed to depth render model %p for light %p, hiding model.", model, scene->light);
				model->visible = false;
				continue;
			}

			rendered++;
		}
	}

	shovelerFramebufferUse(framebuffer);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
		ShovelerModel *model = iter->data;

		if(!model->visible) {
			continue;
		}

		ShovelerShader *shader = getCachedShader(scene, camera, model, model->material);

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

void shovelerSceneFree(ShovelerScene *scene)
{
	shovelerLightFree(scene->light);
	shovelerMaterialFree(scene->depthMaterial);
	g_hash_table_destroy(scene->modelShaderCache);
	for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
		shovelerModelFree(iter->data);
	}
	g_queue_free(scene->models);
	free(scene);
}

static ShovelerShader *generateShader(ShovelerCamera *camera, ShovelerModel *model, ShovelerMaterial *material)
{
	ShovelerShader *shader = shovelerShaderCreate(material);

	int materialAttached = shovelerShaderAttachUniforms(shader, material->uniforms);
	int modelAttached = shovelerShaderAttachUniforms(shader, model->uniforms);
	int cameraAttached = shovelerShaderAttachUniforms(shader, camera->uniforms);

	shovelerLogInfo("Generated shader for camera %p and model %p - attached %d material uniforms, %d model uniforms, and %d camera uniforms.", camera, model, materialAttached, modelAttached, cameraAttached);
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
		shader = generateShader(camera, model, material);
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
