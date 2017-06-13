#include <stdlib.h> // malloc, free
#include <light.h>

#include "camera/identity.h"
#include "filter/depth_texture_gaussian.h"
#include "light/spot.h"
#include "material/depth.h"
#include "scene.h"

static int renderSpotLight(void *spotlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer);
static void freeSpotLight(void *spotlightPointer);

ShovelerLightSpot *shovelerLightSpotCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor)
{
	ShovelerLightSpot *spotlight = malloc(sizeof(ShovelerLightSpot));
	spotlight->light.data = spotlight;
	spotlight->light.render = renderSpotLight;
	spotlight->light.freeData = freeSpotLight;
	spotlight->light.uniforms = shovelerUniformMapCreate();

	spotlight->shadowMapSampler = shovelerSamplerCreate(true, true);
	spotlight->camera = camera;

	spotlight->depthFramebuffer = shovelerFramebufferCreateDepthOnly(width, height, samples);
	spotlight->depthMaterial = shovelerMaterialDepthCreate();
	spotlight->depthFilter = shovelerFilterDepthTextureGaussianCreate(width, height, samples, exponentialFactor);

	spotlight->ambientFactor = ambientFactor;
	spotlight->exponentialFactor = exponentialFactor;
	spotlight->color = (ShovelerVector4){1.0, 1.0, 1.0, 1.0};

	shovelerUniformMapInsert(spotlight->light.uniforms, "isExponentialLiftedShadowMap", shovelerUniformCreateInt(1));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightAmbientFactor", shovelerUniformCreateFloat(spotlight->ambientFactor));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightExponentialShadowFactor", shovelerUniformCreateFloat(spotlight->exponentialFactor));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightColor", shovelerUniformCreateVector4Pointer(&spotlight->color));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightPosition", shovelerUniformCreateVector3Pointer(&spotlight->camera->position));
	shovelerUniformMapInsert(spotlight->light.uniforms, "lightCamera", shovelerUniformCreateMatrixPointer(&spotlight->camera->transformation));
	shovelerUniformMapInsert(spotlight->light.uniforms, "shadowMap", shovelerUniformCreateTexture(spotlight->depthFilter->outputTexture, spotlight->shadowMapSampler));

	return spotlight;
}

static int renderSpotLight(void *spotlightPointer, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	ShovelerLightSpot *spotlight = (ShovelerLightSpot *) spotlightPointer;

	int rendered = 0;

	// render depth map
	shovelerFramebufferUse(spotlight->depthFramebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	rendered += shovelerSceneRenderModels(scene, spotlight->camera, NULL, spotlight->depthMaterial, false, true);

	// filter depth map
	rendered += shovelerFilterRender(spotlight->depthFilter, spotlight->depthFramebuffer->depthTarget);

	// render additive light to scene
	rendered += shovelerSceneRenderPass(scene, camera, &spotlight->light, framebuffer, SHOVELER_SCENE_RENDER_MODE_ADDITIVE_LIGHT);

	return rendered;
}

static void freeSpotLight(void *spotlightPointer)
{
	ShovelerLightSpot *spotlight = (ShovelerLightSpot *) spotlightPointer;

	if(spotlight == NULL) {
		return;
	}

	shovelerFilterFree(spotlight->depthFilter);
	shovelerMaterialFree(spotlight->depthMaterial);
	shovelerFramebufferFree(spotlight->depthFramebuffer);
	shovelerCameraFree(spotlight->camera);
	shovelerSamplerFree(spotlight->shadowMapSampler);
	shovelerUniformMapFree(spotlight->light.uniforms);
	free(spotlight);
}
