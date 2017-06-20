#ifndef SHOVELER_LIGHT_SPOT_H
#define SHOVELER_LIGHT_SPOT_H

#include "camera.h"
#include "filter.h"
#include "framebuffer.h"
#include "light.h"
#include "material.h"
#include "uniform_map.h"
#include "types.h"

typedef struct {
	ShovelerLight light;
	ShovelerSampler *shadowMapSampler;
	ShovelerCamera *camera;
	ShovelerFramebuffer *depthFramebuffer;
	ShovelerMaterial *depthMaterial;
	ShovelerFilter *depthFilter;
	float ambientFactor;
	float exponentialFactor;
	ShovelerVector4 color;
} ShovelerLightSpot;

ShovelerLightSpot *shovelerLightSpotCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector4 color);

#endif
