#include <stdlib.h> // malloc, free

#include "camera.h"

void shovelerCameraInit(ShovelerCamera *camera, void *data, ShovelerCameraFreeDataFunction *freeData)
{
	camera->position = (ShovelerVector3){{0, 0, -5}};
	camera->transformation = shovelerMatrixIdentity;
	camera->uniforms = shovelerUniformMapCreate();
	camera->data = data;
	camera->freeData = freeData;

	ShovelerUniform *cameraUniform = shovelerUniformCreateMatrixPointer(&camera->transformation);
	shovelerUniformMapInsert(camera->uniforms, "camera", cameraUniform);
	ShovelerUniform *cameraPositionUniform = shovelerUniformCreateVector3Pointer(&camera->position);
	shovelerUniformMapInsert(camera->uniforms, "cameraPosition", cameraPositionUniform);
}

void shovelerCameraFree(ShovelerCamera *camera)
{
	shovelerUniformMapFree(camera->uniforms);
	camera->freeData(camera->data);
}
