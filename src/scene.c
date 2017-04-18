#include <stdlib.h> // malloc, free

#include "log.h"
#include "scene.h"
#include "shader.h"

static ShovelerShader *generateShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material);
static ShovelerShader *getCachedShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material);
static void freeHashTable(void *hashTablePointer);
static void freeShader(void *shaderPointer);

ShovelerScene *shovelerSceneCreate()
{
	ShovelerScene *scene = malloc(sizeof(ShovelerScene));
	scene->light = NULL;
	scene->uniforms = shovelerUniformMapCreate();
	scene->models = g_queue_new();
	scene->cameraShaderCache = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeHashTable);
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

int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerMaterial *overrideMaterial, bool onlyShadowCasters)
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

		ShovelerShader *shader = getCachedShader(scene, camera, light, model, overrideMaterial == NULL ? model->material : overrideMaterial);

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

int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerFramebuffer *framebuffer)
{
	int rendered = 0;

	if(light != NULL) {
		rendered += shovelerLightRender(light, scene);
	}

	shovelerFramebufferUse(framebuffer);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rendered += shovelerSceneRenderModels(scene, camera, light, NULL, false);

	return rendered;
}

int shovelerSceneRenderFrame(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	return shovelerSceneRenderPass(scene, camera, scene->light, framebuffer);
}

void shovelerSceneFree(ShovelerScene *scene)
{
	shovelerLightFree(scene->light);
	g_hash_table_destroy(scene->cameraShaderCache);
	for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
		shovelerModelFree(iter->data);
	}
	g_queue_free(scene->models);
	shovelerUniformMapFree(scene->uniforms);
	free(scene);
}

static ShovelerShader *generateShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material)
{
	ShovelerShader *shader = shovelerShaderCreate(material);

	int materialAttached = 0;
	if(material != NULL) {
		materialAttached = shovelerShaderAttachUniforms(shader, material->uniforms);
	}

	int modelAttached = 0;
	if(model != NULL) {
		modelAttached = shovelerShaderAttachUniforms(shader, model->uniforms);
	}

	int lightAttached = 0;
	if(light != NULL) {
		lightAttached = shovelerShaderAttachUniforms(shader, light->uniforms);
	}

	int cameraAttached = 0;
	if(camera != NULL) {
		cameraAttached = shovelerShaderAttachUniforms(shader, camera->uniforms);
	}

	int sceneAttached = 0;
	if(scene != NULL) {
		sceneAttached = shovelerShaderAttachUniforms(shader, scene->uniforms);
	}

	shovelerLogInfo("Generated shader for program %d with %d scene %p, %d camera %p, %d light %p, %d model %p, and %d material %p uniforms.", material->program, sceneAttached, scene, cameraAttached, camera, lightAttached, light, modelAttached, model, materialAttached, material);
	return shader;
}

static ShovelerShader *getCachedShader(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material)
{
	GHashTable *lightShaderCache = g_hash_table_lookup(scene->cameraShaderCache, camera);
	if(lightShaderCache == NULL) {
		lightShaderCache = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeHashTable);
		g_hash_table_insert(scene->cameraShaderCache, camera, lightShaderCache);
	}

	GHashTable *modelShaderCache = g_hash_table_lookup(lightShaderCache, light);
	if(modelShaderCache == NULL) {
		modelShaderCache = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeHashTable);
		g_hash_table_insert(lightShaderCache, light, modelShaderCache);
	}

	GHashTable *materialShaderCache = g_hash_table_lookup(modelShaderCache, model);
	if(materialShaderCache == NULL) {
		materialShaderCache = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeShader);
		g_hash_table_insert(modelShaderCache, model, materialShaderCache);
	}

	ShovelerShader *shader = g_hash_table_lookup(materialShaderCache, material);
	if(shader == NULL) {
		shader = generateShader(scene, camera, light, model, material);
		g_hash_table_insert(materialShaderCache, material, shader);
	}

	return shader;
}

static void freeHashTable(void *hashTablePointer)
{
	g_hash_table_destroy(hashTablePointer);
}

static void freeShader(void *shaderPointer)
{
	shovelerShaderFree(shaderPointer);
}
