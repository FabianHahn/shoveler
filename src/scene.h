#ifndef SHOVELER_SCENE_H
#define SHOVELER_SCENE_H

#include <stdbool.h> // bool

#include <glib.h>

#include "camera.h"
#include "framebuffer.h"
#include "light.h"
#include "material.h"
#include "model.h"

typedef struct {
	ShovelerLight *light;
	ShovelerMaterial *depthMaterial;
	GQueue *models;
	GHashTable *modelShaderCache;
} ShovelerScene;

ShovelerScene *shovelerSceneCreate();
void shovelerSceneAddLight(ShovelerScene *scene, ShovelerLight *light);
void shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model);
int shovelerSceneRender(ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
void shovelerSceneFree(ShovelerScene *scene);

#endif
