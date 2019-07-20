#include <stdlib.h> // malloc, free

#include <glib.h>

#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/shader.h"
#include "shoveler/uniform.h"
#include "shoveler/scene.h"
#include "shoveler/shader_cache.h"

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static int attachUniforms(ShovelerMaterial *material, ShovelerShader *shader, void *userData);
static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialCreate(ShovelerShaderCache *shaderCache, bool screenspace, GLuint program)
{
	ShovelerMaterial *material = shovelerMaterialCreateUnmanaged(shaderCache, screenspace, program);
	material->manageProgram = true;
	return material;
}

ShovelerMaterial *shovelerMaterialCreateUnmanaged(ShovelerShaderCache *shaderCache, bool screenspace, GLuint program)
{
	ShovelerMaterial *material = malloc(sizeof(ShovelerMaterial));
	material->shaderCache = shaderCache;
	material->manageProgram = false;
	material->program = program;
	material->uniforms = shovelerUniformMapCreate();
	material->render = render;
	material->attachUniforms = attachUniforms;
	material->freeData = freeMaterialData;
	material->data = NULL;
	return material;
}

void shovelerMaterialFree(ShovelerMaterial *material)
{
	if(material == NULL) {
		return;
	}

	shovelerShaderCacheInvalidateMaterial(material->shaderCache, material);

	material->freeData(material);
	shovelerUniformMapFree(material->uniforms);

	if(material->manageProgram) {
		glDeleteProgram(material->program);
	}

	free(material);
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	// by default, generate one shader from our shader program, use it, and render the model once
	ShovelerShader *shader = shovelerSceneGenerateShader(scene, camera, light, model, material, NULL);

	if(!shovelerShaderUse(shader)) {
		shovelerLogWarning("Failed to use shader for material %p, scene %p, camera %p, light %p and model %p.", material, scene, camera, light, model);
		return false;
	}

	if(!shovelerModelRender(model)) {
		shovelerLogWarning("Failed to render model %p with material %p in scene %p for camera %p and light %p.", model, material, scene, camera, light);
		return false;
	}

	return true;
}

static int attachUniforms(ShovelerMaterial *material, ShovelerShader *shader, void *userData)
{
	// simply attach uniform map by default
	return shovelerUniformMapAttach(material->uniforms, shader);
}

static void freeMaterialData(ShovelerMaterial *material)
{
	// do nothing by default
}
