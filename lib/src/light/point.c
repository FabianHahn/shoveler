#include <stdlib.h> // malloc, free

#include "shoveler/camera/perspective.h"
#include "shoveler/light/point.h"
#include "shoveler/light/spot.h"
#include "shoveler/constants.h"
#include "shoveler/light.h"
#include "shoveler/scene.h"

static void updatePosition(void *pointlightPointer, ShovelerVector3 position);
static ShovelerVector3 getPosition(void *pointlightPointer);
static int renderPointLight(void *pointlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
static void freePointLight(void *pointlightPointer);

ShovelerLightPoint *shovelerLightPointCreate(ShovelerVector3 position, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color)
{
	ShovelerLightPoint *pointlight = malloc(sizeof(ShovelerLightPoint));
	pointlight->light.data = pointlight;
	pointlight->light.updatePosition = updatePosition;
	pointlight->light.getPosition = getPosition;
	pointlight->light.render = renderPointLight;
	pointlight->light.freeData = freePointLight;
	pointlight->light.uniforms = shovelerUniformMapCreate();
	pointlight->shared = shovelerLightSpotSharedCreate(width, height, samples, ambientFactor, exponentialFactor, color);

	ShovelerCamera *forwardCamera = shovelerCameraPerspectiveCreate(position, (ShovelerVector3){0.0f, 0.0f, 1.0f}, (ShovelerVector3){0.0f, 1.0f, 0.0f}, SHOVELER_PI / 2.0f, 1.0f, 1, 100);
	ShovelerCamera *backwardCamera = shovelerCameraPerspectiveCreate(position, (ShovelerVector3){0.0f, 0.0f, -1.0f}, (ShovelerVector3){0.0f, 1.0f, 0.0f}, SHOVELER_PI / 2.0f, 1.0f, 1, 100);
	ShovelerCamera *leftCamera = shovelerCameraPerspectiveCreate(position, (ShovelerVector3){1.0f, 0.0f, 0.0f}, (ShovelerVector3){0.0f, 1.0f, 0.0f}, SHOVELER_PI / 2.0f, 1.0f, 1, 100);
	ShovelerCamera *rightCamera = shovelerCameraPerspectiveCreate(position, (ShovelerVector3){-1.0f, 0.0f, 0.0f}, (ShovelerVector3){0.0f, 1.0f, 0.0f}, SHOVELER_PI / 2.0f, 1.0f, 1, 100);
	ShovelerCamera *upCamera = shovelerCameraPerspectiveCreate(position, (ShovelerVector3){0.0f, 1.0f, 0.0f}, (ShovelerVector3){0.0f, 0.0f, -1.0f}, SHOVELER_PI / 2.0f, 1.0f, 1, 100);
	ShovelerCamera *downCamera = shovelerCameraPerspectiveCreate(position, (ShovelerVector3){0.0f, -1.0f, 0.0f}, (ShovelerVector3){0.0f, 0.0f, 1.0f}, SHOVELER_PI / 2.0f, 1.0f, 1, 100);

	pointlight->spotlights[0] = shovelerLightSpotCreateWithShared(forwardCamera, pointlight->shared, false);
	pointlight->spotlights[1] = shovelerLightSpotCreateWithShared(backwardCamera, pointlight->shared, false);
	pointlight->spotlights[2] = shovelerLightSpotCreateWithShared(leftCamera, pointlight->shared, false);
	pointlight->spotlights[3] = shovelerLightSpotCreateWithShared(rightCamera, pointlight->shared, false);
	pointlight->spotlights[4] = shovelerLightSpotCreateWithShared(upCamera, pointlight->shared, false);
	pointlight->spotlights[5] = shovelerLightSpotCreateWithShared(downCamera, pointlight->shared, false);

	return pointlight;
}

static void updatePosition(void *pointlightPointer, ShovelerVector3 position)
{
	ShovelerLightPoint *pointlight = pointlightPointer;

	for(int i = 0; i < 6; i++) {
		shovelerLightUpdatePosition(&pointlight->spotlights[i]->light, position);
	}
}

static ShovelerVector3 getPosition(void *pointlightPointer)
{
	ShovelerLightPoint *pointlight = pointlightPointer;
	return pointlight->spotlights[0]->camera->position;
}

static int renderPointLight(void *pointlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	ShovelerLightPoint *pointlight = (ShovelerLightPoint *) pointlightPointer;

	int rendered = 0;

	for(int i = 0; i < 6; i++) {
		rendered += shovelerLightRender(&pointlight->spotlights[i]->light, scene, camera, framebuffer);
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
		shovelerLightFree(&pointlight->spotlights[i]->light);
	}

	shovelerLightSpotSharedFree(pointlight->shared);
	shovelerUniformMapFree(pointlight->light.uniforms);

	free(pointlight);
}
