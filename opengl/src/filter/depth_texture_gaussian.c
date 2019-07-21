#include <stdlib.h> // malloc, free

#include "shoveler/camera/identity.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/filter/depth_texture_gaussian.h"
#include "shoveler/material/depth_texture_gaussian_filter.h"
#include "shoveler/framebuffer.h"
#include "shoveler/material.h"
#include "shoveler/drawable.h"
#include "shoveler/model.h"
#include "shoveler/sampler.h"
#include "shoveler/scene.h"
#include "shoveler/shader_cache.h"

typedef struct {
	ShovelerFilter filter;
	ShovelerShaderCache *shaderCache;
	ShovelerSampler *filterSampler;
	ShovelerCamera *filterCamera;
	ShovelerFramebuffer *filterXFramebuffer;
	ShovelerFramebuffer *filterYFramebuffer;
	ShovelerMaterial *filterXMaterial;
	ShovelerMaterial *filterYMaterial;
	ShovelerDrawable *filterQuad;
	ShovelerModel *filterModel;
	ShovelerScene *filterScene;
	ShovelerSceneRenderPassOptions filterSceneRenderPassOptions;
	float exponentialFactor;
} DepthTextureGaussianFilter;

static int filterDepthTextureGaussian(ShovelerFilter *filter, ShovelerRenderState *renderState);
static void freeDepthTextureGaussian(void *data);

ShovelerFilter *shovelerFilterDepthTextureGaussianCreate(ShovelerShaderCache *shaderCache, int width, int height, GLsizei samples, float exponentialFactor)
{
	DepthTextureGaussianFilter *depthTextureGaussianFilter = malloc(sizeof(DepthTextureGaussianFilter));
	depthTextureGaussianFilter->filter.inputTexture = NULL;
	depthTextureGaussianFilter->filter.data = depthTextureGaussianFilter;
	depthTextureGaussianFilter->filter.filterTexture = filterDepthTextureGaussian;
	depthTextureGaussianFilter->filter.freeData = freeDepthTextureGaussian;

	depthTextureGaussianFilter->shaderCache = shaderCache;
	depthTextureGaussianFilter->filterSampler = shovelerSamplerCreate(false, false, true);
	depthTextureGaussianFilter->filterCamera = shovelerCameraIdentityCreate(shaderCache);

	depthTextureGaussianFilter->filterXFramebuffer = shovelerFramebufferCreateColorOnly(width, height, samples, 1, 32);
	depthTextureGaussianFilter->filterYFramebuffer = shovelerFramebufferCreateColorOnly(width, height, samples, 1, 32);

	depthTextureGaussianFilter->filterXMaterial = shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(shaderCache, &depthTextureGaussianFilter->filter.inputTexture, &depthTextureGaussianFilter->filterSampler, width, height);
	shovelerMaterialDepthTextureGaussianFilterEnableExponentialLifting(depthTextureGaussianFilter->filterXMaterial, exponentialFactor);
	depthTextureGaussianFilter->filterYMaterial = shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(shaderCache, &depthTextureGaussianFilter->filterXFramebuffer->renderTarget, &depthTextureGaussianFilter->filterSampler, width, height);
	shovelerMaterialDepthTextureGaussianFilterSetDirection(depthTextureGaussianFilter->filterYMaterial, false, true);
	depthTextureGaussianFilter->filter.outputTexture = depthTextureGaussianFilter->filterYFramebuffer->renderTarget;

	depthTextureGaussianFilter->filterQuad = shovelerDrawableQuadCreate();
	depthTextureGaussianFilter->filterScene = shovelerSceneCreate(shaderCache);
	depthTextureGaussianFilter->filterModel = shovelerModelCreate(depthTextureGaussianFilter->filterQuad, depthTextureGaussianFilter->filterXMaterial);
	depthTextureGaussianFilter->filterModel->translation.values[0] = -1.0f;
	depthTextureGaussianFilter->filterModel->translation.values[1] = -1.0f;
	depthTextureGaussianFilter->filterModel->scale.values[0] = 2.0f;
	depthTextureGaussianFilter->filterModel->scale.values[1] = 2.0f;
	shovelerModelUpdateTransformation(depthTextureGaussianFilter->filterModel);

	depthTextureGaussianFilter->filterSceneRenderPassOptions.overrideMaterial = NULL;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.emitters = false;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.screenspace = true;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.onlyShadowCasters = true;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.renderState.blend = false;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.renderState.blendSourceFactor = GL_ONE;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.renderState.blendDestinationFactor = GL_ONE;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.renderState.depthTest = false;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.renderState.depthFunction = GL_EQUAL;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.renderState.depthMask = GL_FALSE;
	shovelerSceneAddModel(depthTextureGaussianFilter->filterScene, depthTextureGaussianFilter->filterModel);

	return &depthTextureGaussianFilter->filter;
}

static int filterDepthTextureGaussian(ShovelerFilter *filter, ShovelerRenderState *renderState)
{
	DepthTextureGaussianFilter *depthTextureGaussianFilter = (DepthTextureGaussianFilter *) filter->data;

	int rendered = 0;

	shovelerRenderStateDisableBlend(renderState);
	shovelerRenderStateDisableDepthTest(renderState);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// filter depth map in X direction and lift exponentially
	shovelerFramebufferUse(depthTextureGaussianFilter->filterXFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	depthTextureGaussianFilter->filterSceneRenderPassOptions.overrideMaterial = depthTextureGaussianFilter->filterXMaterial;
	rendered += shovelerSceneRenderPass(depthTextureGaussianFilter->filterScene, depthTextureGaussianFilter->filterCamera, NULL, depthTextureGaussianFilter->filterSceneRenderPassOptions, renderState);

	// filter depth map in Y direction
	shovelerFramebufferUse(depthTextureGaussianFilter->filterYFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	depthTextureGaussianFilter->filterSceneRenderPassOptions.overrideMaterial = depthTextureGaussianFilter->filterYMaterial;
	rendered += shovelerSceneRenderPass(depthTextureGaussianFilter->filterScene, depthTextureGaussianFilter->filterCamera, NULL, depthTextureGaussianFilter->filterSceneRenderPassOptions, renderState);

	return rendered;
}

static void freeDepthTextureGaussian(void *data)
{
	DepthTextureGaussianFilter *depthTextureGaussianFilter = (DepthTextureGaussianFilter *) data;

	shovelerShaderCacheInvalidateCamera(depthTextureGaussianFilter->shaderCache, depthTextureGaussianFilter->filterCamera);

	shovelerSceneFree(depthTextureGaussianFilter->filterScene);
	shovelerDrawableFree(depthTextureGaussianFilter->filterQuad);
	shovelerMaterialFree(depthTextureGaussianFilter->filterYMaterial);
	shovelerMaterialFree(depthTextureGaussianFilter->filterXMaterial);
	shovelerFramebufferFree(depthTextureGaussianFilter->filterYFramebuffer, /* keepTargets */ false);
	shovelerFramebufferFree(depthTextureGaussianFilter->filterXFramebuffer, /* keepTargets */ false);
	shovelerCameraFree(depthTextureGaussianFilter->filterCamera);
	shovelerSamplerFree(depthTextureGaussianFilter->filterSampler);
	free(depthTextureGaussianFilter);
}
