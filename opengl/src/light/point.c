#include <stdlib.h> // malloc, free

#include "shoveler/camera/perspective.h"
#include "shoveler/light/point.h"
#include "shoveler/light/spot.h"
#include "shoveler/constants.h"
#include "shoveler/light.h"
#include "shoveler/projection.h"
#include "shoveler/scene.h"
#include "shoveler/shader_cache.h"
#include "shoveler/types.h"

typedef struct {
	ShovelerLight light;
	ShovelerLightSpotShared *shared;
	ShovelerLight *spotlights[6];
} ShovelerLightPoint;

static void updatePosition(void *pointlightPointer, ShovelerVector3 position);
static ShovelerVector3 getPosition(void *pointlightPointer);
static int renderPointLight(void *pointlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer, ShovelerSceneRenderPassOptions renderPassOptions, ShovelerRenderState *renderState);
static void freePointLight(void *pointlightPointer);

ShovelerLight *shovelerLightPointCreate(ShovelerShaderCache *shaderCache, ShovelerVector3 position, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color)
{
	ShovelerLightPoint *pointlight = malloc(sizeof(ShovelerLightPoint));
	pointlight->light.data = pointlight;
	pointlight->light.updatePosition = updatePosition;
	pointlight->light.getPosition = getPosition;
	pointlight->light.render = renderPointLight;
	pointlight->light.freeData = freePointLight;
	pointlight->light.uniforms = shovelerUniformMapCreate();
	pointlight->shared = shovelerLightSpotSharedCreate(shaderCache, width, height, samples, ambientFactor, exponentialFactor, color);

	ShovelerProjectionPerspective projection;
	projection.fieldOfViewY = SHOVELER_PI / 2.0f;
	projection.aspectRatio = 1.0f;
	projection.nearClippingPlane = 1.0f;
	projection.farClippingPlane = 100.0f;

	ShovelerReferenceFrame forwardFrame = shovelerReferenceFrame(position, shovelerVector3(0.0f, 0.0f, 1.0f), shovelerVector3(0.0f, 1.0f, 0.0f));
	ShovelerReferenceFrame backwardFrame = shovelerReferenceFrame(position, shovelerVector3(0.0f, 0.0f, -1.0f), shovelerVector3(0.0f, 1.0f, 0.0f));
	ShovelerReferenceFrame leftFrame = shovelerReferenceFrame(position, shovelerVector3(1.0f, 0.0f, 0.0f), shovelerVector3(0.0f, 1.0f, 0.0f));
	ShovelerReferenceFrame rightFrame = shovelerReferenceFrame(position, shovelerVector3(-1.0f, 0.0f, 0.0f), shovelerVector3(0.0f, 1.0f, 0.0f));
	ShovelerReferenceFrame upFrame = shovelerReferenceFrame(position, shovelerVector3(0.0f, 1.0f, 0.0f), shovelerVector3(0.0f, 0.0f, -1.0f));
	ShovelerReferenceFrame downFrame = shovelerReferenceFrame(position, shovelerVector3(0.0f, -1.0f, 0.0f), shovelerVector3(0.0f, 0.0f, 1.0f));
	
	ShovelerCamera *forwardCamera = shovelerCameraPerspectiveCreate(shaderCache, &forwardFrame, &projection);
	ShovelerCamera *backwardCamera = shovelerCameraPerspectiveCreate(shaderCache, &backwardFrame, &projection);
	ShovelerCamera *leftCamera = shovelerCameraPerspectiveCreate(shaderCache, &leftFrame, &projection);
	ShovelerCamera *rightCamera = shovelerCameraPerspectiveCreate(shaderCache, &rightFrame, &projection);
	ShovelerCamera *upCamera = shovelerCameraPerspectiveCreate(shaderCache, &upFrame, &projection);
	ShovelerCamera *downCamera = shovelerCameraPerspectiveCreate(shaderCache, &downFrame, &projection);

	pointlight->spotlights[0] = shovelerLightSpotCreateWithShared(forwardCamera, pointlight->shared, false);
	pointlight->spotlights[1] = shovelerLightSpotCreateWithShared(backwardCamera, pointlight->shared, false);
	pointlight->spotlights[2] = shovelerLightSpotCreateWithShared(leftCamera, pointlight->shared, false);
	pointlight->spotlights[3] = shovelerLightSpotCreateWithShared(rightCamera, pointlight->shared, false);
	pointlight->spotlights[4] = shovelerLightSpotCreateWithShared(upCamera, pointlight->shared, false);
	pointlight->spotlights[5] = shovelerLightSpotCreateWithShared(downCamera, pointlight->shared, false);

	return &pointlight->light;
}

ShovelerLightSpotShared *shovelerLightPointGetShared(ShovelerLight *light)
{
	ShovelerLightPoint *pointlight = light->data;
	return pointlight->shared;
}

static void updatePosition(void *pointlightPointer, ShovelerVector3 position)
{
	ShovelerLightPoint *pointlight = pointlightPointer;

	for(int i = 0; i < 6; i++) {
		shovelerLightUpdatePosition(pointlight->spotlights[i], position);
	}
}

static ShovelerVector3 getPosition(void *pointlightPointer)
{
	ShovelerLightPoint *pointlight = pointlightPointer;
	return shovelerLightGetPosition(pointlight->spotlights[0]);
}

static int renderPointLight(void *pointlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer, ShovelerSceneRenderPassOptions renderPassOptions, ShovelerRenderState *renderState)
{
	ShovelerLightPoint *pointlight = (ShovelerLightPoint *) pointlightPointer;

	int rendered = 0;

	for(int i = 0; i < 6; i++) {
		rendered += shovelerLightRender(pointlight->spotlights[i], scene, camera, framebuffer, renderPassOptions, renderState);
	}

	return rendered;
}

static void freePointLight(void *pointlightPointer)
{
	ShovelerLightPoint *pointlight = (ShovelerLightPoint *) pointlightPointer;

	if(pointlight == NULL) {
		return;
	}

	for(int i = 0; i < 6; i++) {
		shovelerLightFree(pointlight->spotlights[i]);
	}

	shovelerLightSpotSharedFree(pointlight->shared);
	shovelerUniformMapFree(pointlight->light.uniforms);

	free(pointlight);
}
