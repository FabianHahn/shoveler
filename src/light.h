#ifndef SHOVELER_LIGHT_H
#define SHOVELER_LIGHT_H

#include "camera.h"
#include "filter.h"
#include "framebuffer.h"
#include "material.h"
#include "uniform_map.h"
#include "types.h"

struct ShovelerSceneStruct; // forward declaration: scene.h

typedef struct {
	ShovelerSampler *shadowMapSampler;
	ShovelerUniformMap *uniforms;
	ShovelerCamera *camera;
	ShovelerFramebuffer *depthFramebuffer;
	ShovelerMaterial *depthMaterial;
	ShovelerFilter *depthFilter;
	float ambientFactor;
	float exponentialFactor;
	ShovelerVector4 color;
} ShovelerLight;

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor);
int shovelerLightRender(ShovelerLight *light, struct ShovelerSceneStruct *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
void shovelerLightFree(ShovelerLight *light);

#endif
