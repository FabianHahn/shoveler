#ifndef SHOVELER_LIGHT_H
#define SHOVELER_LIGHT_H

#include "camera.h"
#include "drawable.h"
#include "framebuffer.h"
#include "material.h"
#include "model.h"
#include "types.h"

struct ShovelerSceneStruct; // forward declaration: scene.h

typedef struct {
	ShovelerCamera *camera;
	ShovelerCamera *filterCamera;
	ShovelerFramebuffer *depthFramebuffer;
	ShovelerFramebuffer *filterXFramebuffer;
	ShovelerFramebuffer *filterYFramebuffer;
	ShovelerMaterial *depthMaterial;
	ShovelerMaterial *filterXMaterial;
	ShovelerMaterial *filterYMaterial;
	ShovelerDrawable *filterQuad;
	ShovelerModel *filterModel;
	struct ShovelerSceneStruct *filterScene;
	float exponentialFactor;
} ShovelerLight;

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float exponentialFactor);
int shovelerLightRender(ShovelerLight *light, struct ShovelerSceneStruct *scene);
void shovelerLightFree(ShovelerLight *light);

#endif
