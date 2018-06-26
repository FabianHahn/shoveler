#ifndef SHOVELER_SCENE_H
#define SHOVELER_SCENE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/camera.h>
#include <shoveler/framebuffer.h>
#include <shoveler/sampler.h>
#include <shoveler/shader.h>
#include <shoveler/uniform_map.h>

struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerModelStruct; // forward declaration: model.h

typedef struct ShovelerSceneStruct {
	ShovelerUniformMap *uniforms;
	struct ShovelerMaterialStruct *depthMaterial;
	GHashTable *lights;
	GHashTable *models;
	GHashTable *shaderCache;
} ShovelerScene;

typedef struct {
	struct ShovelerMaterialStruct *overrideMaterial;
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
bool shovelerSceneAddModel(ShovelerScene *scene, struct ShovelerModelStruct *model);
bool shovelerSceneRemoveModel(ShovelerScene *scene, struct ShovelerModelStruct *model);
int shovelerSceneRenderPass(ShovelerScene *scene, ShovelerCamera *camera, struct ShovelerLightStruct *light, ShovelerSceneRenderPassOptions options);
int shovelerSceneRenderFrame(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
/** Generates a shader, where shaders for calls to this with the same arguments might be cached. */
ShovelerShader *shovelerSceneGenerateShader(ShovelerScene *scene, ShovelerCamera *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, struct ShovelerMaterialStruct *material, void *userData);
void shovelerSceneFree(ShovelerScene *scene);

#endif
