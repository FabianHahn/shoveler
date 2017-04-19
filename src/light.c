#include <stdlib.h> // malloc, free

#include "camera/identity.h"
#include "drawable/quad.h"
#include "material/depth.h"
#include "material/depth_texture_gaussian_filter.h"
#include "light.h"
#include "scene.h"

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor)
{
	ShovelerLight *light = malloc(sizeof(ShovelerLight));
	light->shadowMapSampler = shovelerSamplerCreate(true, true);
	light->uniforms = shovelerUniformMapCreate();
	light->camera = camera;
	light->filterCamera = shovelerCameraIdentityCreate();

	light->depthFramebuffer = shovelerFramebufferCreateDepthOnly(width, height, samples);
	light->filterXFramebuffer = shovelerFramebufferCreate(width, height, samples, 1, 32);
	light->filterYFramebuffer = shovelerFramebufferCreate(width, height, samples, 1, 32);

	light->depthMaterial = shovelerMaterialDepthCreate();
	light->filterXMaterial = shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(light->depthFramebuffer->depthTarget, false, width, height);
	shovelerMaterialDepthTextureGaussianFilterEnableExponentialLifting(light->filterXMaterial, exponentialFactor);
	light->filterYMaterial = shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(light->filterXFramebuffer->renderTarget, false, width, height);
	shovelerMaterialDepthTextureGaussianFilterSetDirection(light->filterYMaterial, false, true);

	light->filterQuad = shovelerDrawableQuadCreate();
	light->filterModel = shovelerModelCreate(light->filterQuad, NULL);
	light->filterModel->screenspace = true;
	light->filterModel->translation.values[0] = -1.0;
	light->filterModel->translation.values[1] = -1.0;
	light->filterModel->scale.values[0] = 2.0;
	light->filterModel->scale.values[1] = 2.0;
	shovelerModelUpdateTransformation(light->filterModel);
	light->filterScene = shovelerSceneCreate();
	shovelerSceneAddModel(light->filterScene, light->filterModel);

	light->ambientFactor = ambientFactor;
	light->exponentialFactor = exponentialFactor;
	light->color = (ShovelerVector4){1.0, 1.0, 1.0, 1.0};

	shovelerUniformMapInsert(light->uniforms, "isExponentialLiftedShadowMap", shovelerUniformCreateInt(1));
	shovelerUniformMapInsert(light->uniforms, "lightAmbientFactor", shovelerUniformCreateFloat(light->ambientFactor));
	shovelerUniformMapInsert(light->uniforms, "lightExponentialShadowFactor", shovelerUniformCreateFloat(light->exponentialFactor));
	shovelerUniformMapInsert(light->uniforms, "lightColor", shovelerUniformCreateVector4Pointer(&light->color));
	shovelerUniformMapInsert(light->uniforms, "lightPosition", shovelerUniformCreateVector3Pointer(&light->camera->position));
	shovelerUniformMapInsert(light->uniforms, "lightCamera", shovelerUniformCreateMatrixPointer(&light->camera->transformation));
	shovelerUniformMapInsert(light->uniforms, "shadowMap", shovelerUniformCreateTexture(light->filterYFramebuffer->renderTarget, light->shadowMapSampler));

	return light;
}

int shovelerLightRender(ShovelerLight *light, ShovelerScene *scene)
{
	int rendered = 0;

	// render depth map
	shovelerFramebufferUse(light->depthFramebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	rendered += shovelerSceneRenderModels(scene, light->camera, NULL, light->depthMaterial, false, true);

	// filter depth map in X direction and lift exponentially
	shovelerFramebufferUse(light->filterXFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rendered += shovelerSceneRenderModels(light->filterScene, light->filterCamera, NULL, light->filterXMaterial, true, true);

	// filter depth map in Y direction
	shovelerFramebufferUse(light->filterYFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rendered += shovelerSceneRenderModels(light->filterScene, light->filterCamera, NULL, light->filterYMaterial, true, true);

	return rendered;
}

void shovelerLightFree(ShovelerLight *light)
{
	if(light == NULL) {
		return;
	}

	shovelerSceneFree(light->filterScene);
	shovelerDrawableFree(light->filterQuad);
	shovelerMaterialFree(light->filterYMaterial);
	shovelerMaterialFree(light->filterXMaterial);
	shovelerMaterialFree(light->depthMaterial);
	shovelerFramebufferFree(light->filterYFramebuffer);
	shovelerFramebufferFree(light->filterXFramebuffer);
	shovelerFramebufferFree(light->depthFramebuffer);
	shovelerCameraFree(light->filterCamera);
	shovelerCameraFree(light->camera);
	shovelerUniformMapFree(light->uniforms);
	shovelerSamplerFree(light->shadowMapSampler);
	free(light);
}
