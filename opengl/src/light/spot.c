#include <stdlib.h> // malloc, free

#include "shoveler/camera/identity.h"
#include "shoveler/filter/depth_texture_gaussian.h"
#include "shoveler/light/spot.h"
#include "shoveler/material/depth.h"
#include "shoveler/scene.h"

typedef struct {
	ShovelerLight light;
	ShovelerCamera *camera;
	ShovelerLightSpotShared *shared;
	bool manageShared;
} ShovelerLightSpot;

static void updatePosition(void *spotlightPointer, ShovelerVector3 position);
static ShovelerVector3 getPosition(void *spotlightPointer);
static int renderSpotLight(void *spotlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer, ShovelerSceneRenderPassOptions renderPassOptions, ShovelerRenderState *renderState);
static void freeSpotLight(void *spotlightPointer);

ShovelerLightSpotShared *shovelerLightSpotSharedCreate(int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color) {
	ShovelerLightSpotShared *shared = malloc(sizeof(ShovelerLightSpotShared));
	shared->shadowMapSampler = shovelerSamplerCreate(true, true, true);
	shared->depthFramebuffer = shovelerFramebufferCreateDepthOnly(width, height, samples);
	shared->depthMaterial = shovelerMaterialDepthCreate();
	shared->depthFilter = shovelerFilterDepthTextureGaussianCreate(width, height, samples, exponentialFactor);
	shared->depthRenderPassOptions.overrideMaterial = shared->depthMaterial;
	shared->depthRenderPassOptions.emitters = false;
	shared->depthRenderPassOptions.screenspace = false;
	shared->depthRenderPassOptions.onlyShadowCasters = true;
	shared->depthRenderPassOptions.renderState.blend = false;
	shared->depthRenderPassOptions.renderState.blendSourceFactor = GL_ONE;
	shared->depthRenderPassOptions.renderState.blendDestinationFactor = GL_ONE;
	shared->depthRenderPassOptions.renderState.depthTest = true;
	shared->depthRenderPassOptions.renderState.depthFunction = GL_LESS;
	shared->depthRenderPassOptions.renderState.depthMask = GL_TRUE;
	shared->ambientFactor = ambientFactor;
	shared->exponentialFactor = exponentialFactor;
	shared->color = color;
	return shared;
}

ShovelerLight *shovelerLightSpotCreateWithShared(ShovelerCamera *camera, ShovelerLightSpotShared *shared, bool managedShared)
{
	ShovelerLightSpot *spotlight = malloc(sizeof(ShovelerLightSpot));
	spotlight->light.data = spotlight;
	spotlight->light.updatePosition = updatePosition;
	spotlight->light.getPosition = getPosition;
	spotlight->light.render = renderSpotLight;
	spotlight->light.freeData = freeSpotLight;
	spotlight->light.uniforms = shovelerUniformMapCreate();
	spotlight->camera = camera;
	spotlight->shared = shared;
	spotlight->manageShared = managedShared;

	shovelerUniformMapInsert(spotlight->light.uniforms, "isExponentialLiftedShadowMap", shovelerUniformCreateInt(1));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightAmbientFactor", shovelerUniformCreateFloat(spotlight->shared->ambientFactor));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightExponentialShadowFactor", shovelerUniformCreateFloat(spotlight->shared->exponentialFactor));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightColor", shovelerUniformCreateVector3Pointer(&spotlight->shared->color));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightPosition", shovelerUniformCreateVector3Pointer(&spotlight->camera->position));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightView", shovelerUniformCreateMatrixPointer(&spotlight->camera->view));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightProjection", shovelerUniformCreateMatrixPointer(&spotlight->camera->projection));
	shovelerUniformMapInsert(spotlight->light.uniforms, "shadowMap", shovelerUniformCreateTexture(spotlight->shared->depthFilter->outputTexture, spotlight->shared->shadowMapSampler));

	return &spotlight->light;
}

void shovelerLightSpotSharedFree(ShovelerLightSpotShared *shared)
{
	if(shared == NULL) {
		return;
	}

	shovelerFilterFree(shared->depthFilter);
	shovelerMaterialFree(shared->depthMaterial);
	shovelerFramebufferFree(shared->depthFramebuffer);
	shovelerSamplerFree(shared->shadowMapSampler);

	free(shared);
}

static void updatePosition(void *spotlightPointer, ShovelerVector3 position)
{
	ShovelerLightSpot *spotlight = (ShovelerLightSpot *) spotlightPointer;
	spotlight->camera->position = position;
	shovelerCameraUpdateView(spotlight->camera);
}

static ShovelerVector3 getPosition(void *spotlightPointer)
{
	ShovelerLightSpot *spotlight = (ShovelerLightSpot *) spotlightPointer;
	return spotlight->camera->position;
}

static int renderSpotLight(void *spotlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer, ShovelerSceneRenderPassOptions renderPassOptions, ShovelerRenderState *renderState)
{
	ShovelerLightSpot *spotlight = (ShovelerLightSpot *) spotlightPointer;

	int rendered = 0;

	// render depth map
	shovelerFramebufferUse(spotlight->shared->depthFramebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	rendered += shovelerSceneRenderPass(scene, spotlight->camera, NULL, spotlight->shared->depthRenderPassOptions, renderState);

	// filter depth map
	rendered += shovelerFilterRender(spotlight->shared->depthFilter, spotlight->shared->depthFramebuffer->depthTarget, renderState);

	// render additive light to scene
	shovelerFramebufferUse(framebuffer);
	rendered += shovelerSceneRenderPass(scene, camera, &spotlight->light, renderPassOptions, renderState);

	return rendered;
}

static void freeSpotLight(void *spotlightPointer)
{
	ShovelerLightSpot *spotlight = (ShovelerLightSpot *) spotlightPointer;

	if(spotlight == NULL) {
		return;
	}

	shovelerCameraFree(spotlight->camera);
	shovelerUniformMapFree(spotlight->light.uniforms);

	if(spotlight->manageShared) {
		shovelerLightSpotSharedFree(spotlight->shared);
	}

	free(spotlight);
}
