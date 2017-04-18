#ifndef SHOVELER_SCENE_H
#define SHOVELER_SCENE_H

#include <stdbool.h> // bool

#include <glib.h>

#include "camera.h"
#include "framebuffer.h"
#include "light.h"
#include "material.h"
#include "model.h"
#include "sampler.h"
#include "uniform_map.h"

typedef struct ShovelerSceneStruct {
	ShovelerLight *light;
	ShovelerUniformMap *uniforms;
	GQueue *models;
	GHashTable *cameraShaderCache;
} ShovelerScene;

ShovelerScene *shovelerSceneCreate();
void shovelerSceneAddLight(ShovelerScene *scene, ShovelerLight *light);
void shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model);
int shovelerSceneRenderModels(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerMaterial *overrideMaterial, bool onlyShadowCasters);
int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerFramebuffer *framebuffer);
int shovelerSceneRenderFrame(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
void shovelerSceneFree(ShovelerScene *scene);

#endif
