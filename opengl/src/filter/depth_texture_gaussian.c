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

typedef struct {
	ShovelerFilter filter;
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

static int filterDepthTextureGaussian(ShovelerFilter *filter);
static void freeDepthTextureGaussian(void *data);

ShovelerFilter *shovelerFilterDepthTextureGaussianCreate(int width, int height, GLsizei samples, float exponentialFactor)
{
	DepthTextureGaussianFilter *depthTextureGaussianFilter = malloc(sizeof(DepthTextureGaussianFilter));
	depthTextureGaussianFilter->filter.inputTexture = NULL;
	depthTextureGaussianFilter->filter.data = depthTextureGaussianFilter;
	depthTextureGaussianFilter->filter.filterTexture = filterDepthTextureGaussian;
	depthTextureGaussianFilter->filter.freeData = freeDepthTextureGaussian;

	depthTextureGaussianFilter->filterSampler = shovelerSamplerCreate(false, true);
	depthTextureGaussianFilter->filterCamera = shovelerCameraIdentityCreate();

	depthTextureGaussianFilter->filterXFramebuffer = shovelerFramebufferCreateColorOnly(width, height, samples, 1, 32);
	depthTextureGaussianFilter->filterYFramebuffer = shovelerFramebufferCreateColorOnly(width, height, samples, 1, 32);

	depthTextureGaussianFilter->filterXMaterial = shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(&depthTextureGaussianFilter->filter.inputTexture, &depthTextureGaussianFilter->filterSampler, width, height);
	shovelerMaterialDepthTextureGaussianFilterEnableExponentialLifting(depthTextureGaussianFilter->filterXMaterial, exponentialFactor);
	depthTextureGaussianFilter->filterYMaterial = shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(&depthTextureGaussianFilter->filterXFramebuffer->renderTarget, &depthTextureGaussianFilter->filterSampler, width, height);
	shovelerMaterialDepthTextureGaussianFilterSetDirection(depthTextureGaussianFilter->filterYMaterial, false, true);
	depthTextureGaussianFilter->filter.outputTexture = depthTextureGaussianFilter->filterYFramebuffer->renderTarget;

	depthTextureGaussianFilter->filterQuad = shovelerDrawableQuadCreate();
	depthTextureGaussianFilter->filterModel = shovelerModelCreate(depthTextureGaussianFilter->filterQuad, NULL);
	depthTextureGaussianFilter->filterModel->screenspace = true;
	depthTextureGaussianFilter->filterModel->translation.values[0] = -1.0f;
	depthTextureGaussianFilter->filterModel->translation.values[1] = -1.0f;
	depthTextureGaussianFilter->filterModel->scale.values[0] = 2.0f;
	depthTextureGaussianFilter->filterModel->scale.values[1] = 2.0f;
	shovelerModelUpdateTransformation(depthTextureGaussianFilter->filterModel);

	depthTextureGaussianFilter->filterScene = shovelerSceneCreate();
	depthTextureGaussianFilter->filterSceneRenderPassOptions.overrideMaterial = NULL;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.emitters = false;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.screenspace = true;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.onlyShadowCasters = true;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.blend = false;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.depthTest = false;
	depthTextureGaussianFilter->filterSceneRenderPassOptions.depthMask = GL_FALSE;
	shovelerSceneAddModel(depthTextureGaussianFilter->filterScene, depthTextureGaussianFilter->filterModel);

	return &depthTextureGaussianFilter->filter;
}

static int filterDepthTextureGaussian(ShovelerFilter *filter)
{
	DepthTextureGaussianFilter *depthTextureGaussianFilter = (DepthTextureGaussianFilter *) filter->data;

	int rendered = 0;

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// filter depth map in X direction and lift exponentially
	shovelerFramebufferUse(depthTextureGaussianFilter->filterXFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	depthTextureGaussianFilter->filterSceneRenderPassOptions.overrideMaterial = depthTextureGaussianFilter->filterXMaterial;
	rendered += shovelerSceneRenderPass(depthTextureGaussianFilter->filterScene, depthTextureGaussianFilter->filterCamera, NULL, depthTextureGaussianFilter->filterSceneRenderPassOptions);

	// filter depth map in Y direction
	shovelerFramebufferUse(depthTextureGaussianFilter->filterYFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	depthTextureGaussianFilter->filterSceneRenderPassOptions.overrideMaterial = depthTextureGaussianFilter->filterYMaterial;
	rendered += shovelerSceneRenderPass(depthTextureGaussianFilter->filterScene, depthTextureGaussianFilter->filterCamera, NULL, depthTextureGaussianFilter->filterSceneRenderPassOptions);

	return rendered;
}

static void freeDepthTextureGaussian(void *data)
{
	DepthTextureGaussianFilter *depthTextureGaussianFilter = (DepthTextureGaussianFilter *) data;
	shovelerSceneFree(depthTextureGaussianFilter->filterScene);
	shovelerDrawableFree(depthTextureGaussianFilter->filterQuad);
	shovelerMaterialFree(depthTextureGaussianFilter->filterYMaterial);
	shovelerMaterialFree(depthTextureGaussianFilter->filterXMaterial);
	shovelerFramebufferFree(depthTextureGaussianFilter->filterYFramebuffer);
	shovelerFramebufferFree(depthTextureGaussianFilter->filterXFramebuffer);
	shovelerCameraFree(depthTextureGaussianFilter->filterCamera);
	shovelerSamplerFree(depthTextureGaussianFilter->filterSampler);
	free(depthTextureGaussianFilter);
}
