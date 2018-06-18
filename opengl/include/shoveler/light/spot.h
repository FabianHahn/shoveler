#ifndef SHOVELER_LIGHT_SPOT_H
#define SHOVELER_LIGHT_SPOT_H

#include <shoveler/camera.h>
#include <shoveler/filter.h>
#include <shoveler/framebuffer.h>
#include <shoveler/light.h>
#include <shoveler/material.h>
#include <shoveler/uniform_map.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerSampler *shadowMapSampler;
	ShovelerFramebuffer *depthFramebuffer;
	ShovelerMaterial *depthMaterial;
	ShovelerFilter *depthFilter;
	ShovelerSceneRenderPassOptions depthRenderPassOptions;
	float ambientFactor;
	float exponentialFactor;
	ShovelerVector3 color;
} ShovelerLightSpotShared;

ShovelerLightSpotShared *shovelerLightSpotSharedCreate(int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color);
ShovelerLight *shovelerLightSpotCreateWithShared(ShovelerCamera *camera, ShovelerLightSpotShared *shared, bool managedShared);
void shovelerLightSpotSharedFree(ShovelerLightSpotShared *shared);

static inline ShovelerLight *shovelerLightSpotCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color)
{
	ShovelerLightSpotShared *shared = shovelerLightSpotSharedCreate(width, height, samples, ambientFactor, exponentialFactor, color);
	return shovelerLightSpotCreateWithShared(camera, shared, true);
}

#endif
