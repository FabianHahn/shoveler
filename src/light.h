#ifndef SHOVELER_LIGHT_H
#define SHOVELER_LIGHT_H

#include "camera.h"
#include "framebuffer.h"
#include "types.h"

typedef struct {
	ShovelerCamera *camera;
	ShovelerFramebuffer *framebuffer;
} ShovelerLight;

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height);
void shovelerLightFree(ShovelerLight *light);

#endif
