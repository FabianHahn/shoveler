#ifndef SHOVELER_LIGHT_H
#define SHOVELER_LIGHT_H

#include <shoveler/camera.h>
#include <shoveler/filter.h>
#include <shoveler/framebuffer.h>
#include <shoveler/material.h>
#include <shoveler/uniform_map.h>
#include <shoveler/types.h>

struct ShovelerSceneStruct; // forward declaration: scene.h

typedef void (ShovelerLightUpdatePositionFunction)(void *data, ShovelerVector3 position);
typedef ShovelerVector3 (ShovelerLightGetPositionFunction)(void *data);
typedef int (ShovelerLightRenderFunction)(void *data, struct ShovelerSceneStruct *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
typedef void (ShovelerLightFreeDataFunction)(void *data);

typedef struct {
	ShovelerUniformMap *uniforms;
	void *data;
	ShovelerLightUpdatePositionFunction *updatePosition;
	ShovelerLightGetPositionFunction *getPosition;
	ShovelerLightRenderFunction *render;
	ShovelerLightFreeDataFunction *freeData;
} ShovelerLight;

static inline void shovelerLightUpdatePosition(ShovelerLight *light, ShovelerVector3 position)
{
	light->updatePosition(light->data, position);
}

static inline ShovelerVector3 shovelerLightGetPosition(ShovelerLight *light)
{
	return light->getPosition(light->data);
}

static inline int shovelerLightRender(ShovelerLight *light, struct ShovelerSceneStruct *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	return light->render(light->data, scene, camera, framebuffer);
}

static inline void shovelerLightFree(ShovelerLight *light)
{
	light->freeData(light->data);
}

#endif
