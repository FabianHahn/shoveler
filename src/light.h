#ifndef SHOVELER_LIGHT_H
#define SHOVELER_LIGHT_H

#include "camera.h"
#include "framebuffer.h"
#include "material.h"
#include "types.h"

struct ShovelerSceneStruct; // forward declaration: scene.h

typedef struct {
	ShovelerCamera *camera;
	ShovelerFramebuffer *framebuffer;
	ShovelerMaterial *depthMaterial;
} ShovelerLight;

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height, GLsizei samples);
int shovelerLightRender(ShovelerLight *light, struct ShovelerSceneStruct *scene);
void shovelerLightFree(ShovelerLight *light);

#endif
