#ifndef SHOVELER_SCENE_H
#define SHOVELER_SCENE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/camera.h>
#include <shoveler/framebuffer.h>
#include <shoveler/light.h>
#include <shoveler/material.h>
#include <shoveler/model.h>
#include <shoveler/sampler.h>
#include <shoveler/uniform_map.h>

typedef struct ShovelerSceneStruct {
	ShovelerUniformMap *uniforms;
	ShovelerMaterial *depthMaterial;
	GHashTable *lights;
	GHashTable *models;
	GHashTable *cameraShaderCache;
} ShovelerScene;

typedef enum {
	SHOVELER_SCENE_RENDER_MODE_OCCLUDED,
	SHOVELER_SCENE_RENDER_MODE_EMITTERS,
	SHOVELER_SCENE_RENDER_MODE_SCREENSPACE,
	SHOVELER_SCENE_RENDER_MODE_ADDITIVE_LIGHT,
} ShovelerSceneRenderMode;

ShovelerScene *shovelerSceneCreate();
bool shovelerSceneAddLight(ShovelerScene *scene, ShovelerLight *light);
bool shovelerSceneRemoveLight(ShovelerScene *scene, ShovelerLight *light);
bool shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model);
bool shovelerSceneRemoveModel(ShovelerScene *scene, ShovelerModel *model);
int shovelerSceneRenderModels(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerMaterial *overrideMaterial, bool emitters, bool screenspace, bool onlyShadowCasters);
int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerFramebuffer *framebuffer, ShovelerSceneRenderMode renderMode);
int shovelerSceneRenderFrame(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
void shovelerSceneFree(ShovelerScene *scene);

#endif
