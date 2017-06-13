#include <stdlib.h> // malloc, free

#include "camera/identity.h"
#include "filter/depth_texture_gaussian.h"
#include "material/depth.h"
#include "light.h"
#include "scene.h"
#include "framebuffer.h"

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor)
{
	ShovelerLight *light = malloc(sizeof(ShovelerLight));
	light->shadowMapSampler = shovelerSamplerCreate(true, true);
	light->uniforms = shovelerUniformMapCreate();
	light->camera = camera;

	light->depthFramebuffer = shovelerFramebufferCreateDepthOnly(width, height, samples);
	light->depthMaterial = shovelerMaterialDepthCreate();
	light->depthFilter = shovelerFilterDepthTextureGaussianCreate(width, height, samples, exponentialFactor);

	light->ambientFactor = ambientFactor;
	light->exponentialFactor = exponentialFactor;
	light->color = (ShovelerVector4){1.0, 1.0, 1.0, 1.0};

	shovelerUniformMapInsert(light->uniforms, "isExponentialLiftedShadowMap", shovelerUniformCreateInt(1));
	shovelerUniformMapInsert(light->uniforms, "lightAmbientFactor", shovelerUniformCreateFloat(light->ambientFactor));
	shovelerUniformMapInsert(light->uniforms, "lightExponentialShadowFactor", shovelerUniformCreateFloat(light->exponentialFactor));
	shovelerUniformMapInsert(light->uniforms, "lightColor", shovelerUniformCreateVector4Pointer(&light->color));
	shovelerUniformMapInsert(light->uniforms, "lightPosition", shovelerUniformCreateVector3Pointer(&light->camera->position));
	shovelerUniformMapInsert(light->uniforms, "lightCamera", shovelerUniformCreateMatrixPointer(&light->camera->transformation));
	shovelerUniformMapInsert(light->uniforms, "shadowMap", shovelerUniformCreateTexture(light->depthFilter->outputTexture, light->shadowMapSampler));

	return light;
}

int shovelerLightRender(ShovelerLight *light, ShovelerScene *scene, ShovelerCamera *camera, ShovelerFramebuffer *framebuffer)
{
	int rendered = 0;

	// render depth map
	shovelerFramebufferUse(light->depthFramebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	rendered += shovelerSceneRenderModels(scene, light->camera, NULL, light->depthMaterial, false, true);

	// filter depth map
	rendered += shovelerFilterRender(light->depthFilter, light->depthFramebuffer->depthTarget);

	// render additive light to scene
	rendered += shovelerSceneRenderPass(scene, camera, light, framebuffer, SHOVELER_SCENE_RENDER_MODE_ADDITIVE_LIGHT);

	return rendered;
}

void shovelerLightFree(ShovelerLight *light)
{
	if(light == NULL) {
		return;
	}

	shovelerFilterFree(light->depthFilter);
	shovelerMaterialFree(light->depthMaterial);
	shovelerFramebufferFree(light->depthFramebuffer);
	shovelerCameraFree(light->camera);
	shovelerUniformMapFree(light->uniforms);
	shovelerSamplerFree(light->shadowMapSampler);
	free(light);
}
