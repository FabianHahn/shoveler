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
	ShovelerSampler *shadowMapSampler;
	ShovelerFramebuffer *depthFramebuffer;
	ShovelerMaterial *depthMaterial;
	ShovelerFilter *depthFilter;
	float ambientFactor;
	float exponentialFactor;
	ShovelerVector3 color;
} ShovelerLightSpotShared;

typedef struct {
	ShovelerLight light;
	ShovelerCamera *camera;
	ShovelerLightSpotShared *shared;
	bool manageShared;
} ShovelerLightSpot;

ShovelerLightSpotShared *shovelerLightSpotSharedCreate(int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color);
ShovelerLightSpot *shovelerLightSpotCreateWithShared(ShovelerCamera *camera, ShovelerLightSpotShared *shared, bool managedShared);
void shovelerLightSpotSharedFree(ShovelerLightSpotShared *shared);

static inline ShovelerLightSpot *shovelerLightSpotCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color)
{
	ShovelerLightSpotShared *shared = shovelerLightSpotSharedCreate(width, height, samples, ambientFactor, exponentialFactor, color);
	return shovelerLightSpotCreateWithShared(camera, shared, true);
}

#endif
