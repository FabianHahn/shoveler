#include <stdlib.h> // malloc, free

#include "camera.h"

ShovelerCamera *shovelerCameraCreate()
{
	ShovelerCamera *camera = malloc(sizeof(ShovelerCamera));
	camera->position = (ShovelerVector3){{0, 0, 0}};
	camera->transformation = shovelerMatrixIdentity;
	camera->uniforms = shovelerUniformMapCreate();

	ShovelerUniform *cameraUniform = shovelerUniformCreateMatrixPointer(&camera->transformation);
	shovelerUniformMapInsert(camera->uniforms, "camera", cameraUniform);
	ShovelerUniform *cameraPositionUniform = shovelerUniformCreateVector3Pointer(&camera->position);
	shovelerUniformMapInsert(camera->uniforms, "cameraPosition", cameraPositionUniform);

	return camera;
}

void shovelerCameraFree(ShovelerCamera *camera)
{
	shovelerUniformMapFree(camera->uniforms);
	free(camera);
}
