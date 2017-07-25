#ifndef SHOVELER_LIGHT_H
#define SHOVELER_LIGHT_H

#include "camera.h"
#include "filter.h"
#include "framebuffer.h"
#include "material.h"
#include "uniform_map.h"
#include "types.h"

struct ShovelerSceneStruct; // forward declaration: scene.h

typedef int (ShovelerLightRenderFunction)(void *data, struct ShovelerSceneStruct *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
typedef void (ShovelerLightFreeDataFunction)(void *data);

typedef struct {
	void *data;
	ShovelerUniformMap *uniforms;
	ShovelerLightRenderFunction *render;
	ShovelerLightFreeDataFunction *freeData;
} ShovelerLight;

static inline int shovelerLightRender(ShovelerLight *light, struct ShovelerSceneStruct *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	return light->render(light->data, scene, camera, framebuffer);
}

static inline void shovelerLightFree(ShovelerLight *light)
{
	light->freeData(light->data);
}

#endif
