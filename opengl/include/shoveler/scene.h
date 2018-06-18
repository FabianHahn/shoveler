#ifndef SHOVELER_SCENE_H
#define SHOVELER_SCENE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/camera.h>
#include <shoveler/framebuffer.h>
#include <shoveler/material.h>
#include <shoveler/model.h>
#include <shoveler/sampler.h>
#include <shoveler/uniform_map.h>

struct ShovelerLightStruct; // forward declaration: light.h

typedef struct ShovelerSceneStruct {
	ShovelerUniformMap *uniforms;
	ShovelerMaterial *depthMaterial;
	GHashTable *lights;
	GHashTable *models;
	GHashTable *cameraShaderCache;
} ShovelerScene;

typedef struct {
	ShovelerMaterial *overrideMaterial;
	bool emitters;
	bool screenspace;
	bool onlyShadowCasters;
	bool blend;
	GLenum blendSourceFactor;
	GLenum blendDestinationFactor;
	bool depthTest;
	GLenum depthFunction;
	GLboolean depthMask;
} ShovelerSceneRenderPassOptions;

ShovelerScene *shovelerSceneCreate();
bool shovelerSceneAddLight(ShovelerScene *scene, struct ShovelerLightStruct *light);
bool shovelerSceneRemoveLight(ShovelerScene *scene, struct ShovelerLightStruct *light);
bool shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model);
bool shovelerSceneRemoveModel(ShovelerScene *scene, ShovelerModel *model);
int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, struct ShovelerLightStruct *light, ShovelerSceneRenderPassOptions options);
int shovelerSceneRenderFrame(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
void shovelerSceneFree(ShovelerScene *scene);

#endif
