#ifndef SHOVELER_SCENE_H
#define SHOVELER_SCENE_H

#include <stdbool.h> // bool

#include <glib.h>

#include "camera.h"
#include "model.h"

typedef struct {
	GQueue *models;
	GHashTable *modelShaderCache;
} ShovelerScene;

ShovelerScene *shovelerSceneCreate();
bool shovelerSceneAddModel(ShovelerScene *scene, ShovelerModel *model);
int shovelerSceneRender(ShovelerScene *scene, ShovelerCamera *camera);
void shovelerSceneFree(ShovelerScene *scene);

#endif
