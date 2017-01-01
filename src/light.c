#include <stdlib.h> // malloc, free

#include "light.h"

ShovelerLight *shovelerLightCreate(ShovelerCamera *camera, int width, int height, GLsizei samples)
{
	ShovelerLight *light = malloc(sizeof(ShovelerLight));
	light->camera = camera;
	light->framebuffer = shovelerFramebufferCreateDepthOnly(width, height, samples);
	return light;
}

void shovelerLightFree(ShovelerLight *light)
{
	if(light == NULL) {
		return;
	}

	shovelerFramebufferFree(light->framebuffer);
	shovelerCameraFree(light->camera);
	free(light);
}
