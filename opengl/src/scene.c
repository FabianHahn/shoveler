#include <stdlib.h> // malloc, free

#include "shoveler/material/depth.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/scene.h"
#include "shoveler/shader.h"

typedef enum {
	RENDER_MODE_OCCLUDED,
	RENDER_MODE_EMITTERS,
	RENDER_MODE_SCREENSPACE,
	RENDER_MODE_ADDITIVE_LIGHT,
} RenderMode;

ShovelerSceneRenderPassOptions createRenderPassOptions(ShovelerScene *scene, RenderMode renderMode);
static void freeLight(void *lightPointer);
static void freeModel(void *modelPointer);
static void freeShader(void *shaderPointer);

ShovelerScene *shovelerSceneCreate()
{
	ShovelerScene *scene = malloc(sizeof(ShovelerScene));
	scene->uniforms = shovelerUniformMapCreate();
	scene->depthMaterial = shovelerMaterialDepthCreate();
	scene->lights = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeLight, NULL);
	scene->models = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeModel, NULL);
	scene->shaderCache = g_hash_table_new_full(shovelerShaderKeyHash, shovelerShaderKeyEqual, NULL, freeShader);
	return scene;
}

bool shovelerSceneAddLight(ShovelerScene *scene, ShovelerLight *light)
{
	return g_hash_table_add(scene->lights, light);
}

bool shovelerSceneRemoveLight(ShovelerScene *scene, ShovelerLight *light)
{
	// TODO: cleanup shader cache entries
	return g_hash_table_remove(scene->lights, light);
}

bool shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model)
{
	return g_hash_table_add(scene->models, model);
}

bool shovelerSceneRemoveModel(ShovelerScene *scene, ShovelerModel *model)
{
	// TODO: cleanup shader cache entries
	return g_hash_table_remove(scene->models, model);
}

int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerSceneRenderPassOptions options)
{
	int rendered = 0;

	if(options.blend) {
		glEnable(GL_BLEND);
		glBlendFunc(options.blendSourceFactor, options.blendDestinationFactor);
	} else {
		glDisable(GL_BLEND);
	}

	if(options.depthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(options.depthFunction);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	glDepthMask(options.depthMask);

	GHashTableIter iter;
	ShovelerModel *model;
	g_hash_table_iter_init(&iter, scene->models);
	while(g_hash_table_iter_next(&iter, (gpointer *) &model, NULL)) {
		if(!model->visible) {
			continue;
		}

		if(model->emitter != options.emitters) {
			continue;
		}

		if(model->screenspace != options.screenspace) {
			continue;
		}

		if(options.onlyShadowCasters && !model->screenspace && !model->castsShadow) {
			continue;
		}

		ShovelerMaterial *material = options.overrideMaterial == NULL ? model->material : options.overrideMaterial;
		if(!shovelerMaterialRender(material, scene, camera, light, model)) {
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

	shovelerFramebufferUse(framebuffer);

	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	rendered += shovelerSceneRenderPass(scene, camera, NULL, createRenderPassOptions(scene, RENDER_MODE_OCCLUDED));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GHashTableIter iter;
	ShovelerLight *light;
	g_hash_table_iter_init(&iter, scene->lights);
	while(g_hash_table_iter_next(&iter, (gpointer *) &light, NULL)) {
		rendered += shovelerLightRender(light, scene, camera, framebuffer, createRenderPassOptions(scene, RENDER_MODE_ADDITIVE_LIGHT));
	}

	rendered += shovelerSceneRenderPass(scene, camera, NULL, createRenderPassOptions(scene, RENDER_MODE_EMITTERS));
	rendered += shovelerSceneRenderPass(scene, camera, NULL, createRenderPassOptions(scene, RENDER_MODE_SCREENSPACE));

	return rendered;
}

ShovelerShader *shovelerSceneGenerateShader(ShovelerScene *scene, ShovelerCamera *camera, struct ShovelerLightStruct *light, ShovelerModel *model, ShovelerMaterial *material, void *userData)
{
	ShovelerShaderKey shaderKey = {scene, camera, light, model, material, userData};

	ShovelerShader *shader = g_hash_table_lookup(scene->shaderCache, &shaderKey);
	if(shader == NULL) {
		shader = shovelerShaderCreate(shaderKey, material);

		int materialAttached = 0;
		if(material != NULL) {
			materialAttached = shovelerMaterialAttachUniforms(material, shader, userData);
		}

		int modelAttached = 0;
		if(model != NULL) {
			modelAttached = shovelerUniformMapAttach(model->uniforms, shader);
		}

		int lightAttached = 0;
		if(light != NULL) {
			lightAttached = shovelerUniformMapAttach(light->uniforms, shader);
		}

		int cameraAttached = 0;
		if(camera != NULL) {
			cameraAttached = shovelerUniformMapAttach(camera->uniforms, shader);
		}

		int sceneAttached = 0;
		if(scene != NULL) {
			sceneAttached = shovelerUniformMapAttach(scene->uniforms, shader);
		}

		shovelerLogInfo("Generated shader for program %d with %d scene %p, %d camera %p, %d light %p, %d model %p, and %d material %p uniforms.", material->program, sceneAttached, scene, cameraAttached, camera, lightAttached, light, modelAttached, model, materialAttached, material);

		g_hash_table_insert(scene->shaderCache, &shader->key, shader);
	}

	return shader;
}

void shovelerSceneFree(ShovelerScene *scene)
{
	g_hash_table_destroy(scene->shaderCache);
	g_hash_table_destroy(scene->models);
	g_hash_table_destroy(scene->lights);
	shovelerMaterialFree(scene->depthMaterial);
	shovelerUniformMapFree(scene->uniforms);
	free(scene);
}

ShovelerSceneRenderPassOptions createRenderPassOptions(ShovelerScene *scene, RenderMode renderMode)
{
	ShovelerSceneRenderPassOptions options;
	options.overrideMaterial = NULL;
	options.emitters = false;
	options.screenspace = false;
	options.onlyShadowCasters = false;
	options.blend = true;
	options.blendSourceFactor = GL_ONE;
	options.blendDestinationFactor = GL_ZERO;
	options.depthTest = true;
	options.depthFunction = GL_LESS;
	options.depthMask = GL_TRUE;
	switch(renderMode) {
		case RENDER_MODE_OCCLUDED:
			options.blendSourceFactor = GL_ONE;
			options.blendDestinationFactor = GL_ZERO;
			options.depthFunction = GL_LESS;
			options.depthMask = GL_TRUE;
		break;
		case RENDER_MODE_EMITTERS:
			options.emitters = true;
			options.blendSourceFactor = GL_ONE;
			options.blendDestinationFactor = GL_ONE;
			options.depthFunction = GL_LESS;
			options.depthMask = GL_FALSE;
		break;
		case RENDER_MODE_SCREENSPACE:
			options.screenspace = true;
			options.blendSourceFactor = GL_ONE;
			options.blendDestinationFactor = GL_ZERO;
			options.depthFunction = GL_ALWAYS;
			options.depthMask = GL_TRUE;
		break;
		case RENDER_MODE_ADDITIVE_LIGHT:
			options.blendSourceFactor = GL_ONE;
			options.blendDestinationFactor = GL_ONE;
			options.depthFunction = GL_EQUAL;
			options.depthMask = GL_TRUE;
		break;
	}

	if(renderMode == RENDER_MODE_OCCLUDED) {
		options.overrideMaterial = scene->depthMaterial;
	}

	return options;
}

static void freeLight(void *lightPointer)
{
	ShovelerLight *light = lightPointer;
	shovelerLightFree(light);
}

static void freeModel(void *modelPointer)
{
	ShovelerModel *model = modelPointer;
	shovelerModelFree(model);
}

static void freeHashTable(void *hashTablePointer)
{
	g_hash_table_destroy(hashTablePointer);
}

static void freeShader(void *shaderPointer)
{
	shovelerShaderFree(shaderPointer);
}
