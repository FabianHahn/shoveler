#include <stdlib.h> // malloc, free

#include "log.h"
#include "scene.h"
#include "shader.h"

static ShovelerShader *generateShader(ShovelerCamera *camera, ShovelerModel *model);

ShovelerScene *shovelerSceneCreate()
{
	ShovelerScene *scene = malloc(sizeof(ShovelerScene));
	scene->models = g_queue_new();
	return scene;
}

bool shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model)
{
	g_queue_push_tail(scene->models, model);
}

int shovelerSceneRender(ShovelerScene *scene, ShovelerCamera *camera)
{
	int rendered = 0;

	for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
		ShovelerModel *model = iter->data;

		if(!model->visible) {
			continue;
		}

		ShovelerShader *shader = generateShader(camera, model);

		if(!shovelerShaderUse(shader)) {
			shovelerLogWarning("Failed to use shader for model %p and camera %p, hiding model.", model, camera);
			model->visible = false;
			shovelerShaderFree(shader);
		}

		if(!shovelerModelRender(model)) {
			shovelerLogWarning("Failed to render model %p with camera %p, hiding model.", model, camera);
			model->visible = false;
			shovelerShaderFree(shader);
			continue;
		}

		shovelerShaderFree(shader);

		rendered++;
	}

	return rendered;
}

void shovelerSceneFree(ShovelerScene *scene)
{
	for(GList *iter = scene->models->head; iter != NULL; iter = iter->next) {
		shovelerModelFree(iter->data);
	}
	g_queue_free(scene->models);
	free(scene);
}

static ShovelerShader *generateShader(ShovelerCamera *camera, ShovelerModel *model)
{
	ShovelerShader *shader = shovelerShaderCreate(model->material->program);

	int materialAttached = shovelerShaderAttachUniforms(shader, model->material->uniforms);
	int modelAttached = shovelerShaderAttachUniforms(shader, model->uniforms);
	int cameraAttached = shovelerShaderAttachUniforms(shader, camera->uniforms);

	shovelerLogInfo("Generated shader for camera %p and model %p - attached %d material uniforms, %d model uniforms, and %d camera uniforms.", camera, model, materialAttached, modelAttached, cameraAttached);
	return shader;
}
