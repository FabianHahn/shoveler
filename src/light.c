#include <stdlib.h> // malloc, free

#include "material/depth.h"
#include "light.h"
#include "scene.h"

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height, GLsizei samples)
{
	ShovelerLight *light = malloc(sizeof(ShovelerLight));
	light->camera = camera;
	light->framebuffer = shovelerFramebufferCreateDepthOnly(width, height, samples);
	light->depthMaterial = shovelerMaterialDepthCreate();
	return light;
}

int shovelerLightRender(ShovelerLight *light, ShovelerScene *scene)
{
	shovelerFramebufferUse(scene->light->framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	return shovelerSceneRenderPass(scene, light->camera, light->depthMaterial, true);
}

void shovelerLightFree(ShovelerLight *light)
{
	if(light == NULL) {
		return;
	}

	shovelerMaterialFree(light->depthMaterial);
	shovelerFramebufferFree(light->framebuffer);
	shovelerCameraFree(light->camera);
	free(light);
}
