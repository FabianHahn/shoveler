#include <stdlib.h> // malloc, free

#include "camera.h"

void shovelerCameraInit(ShovelerCamera *camera, ShovelerVector3 position, void *data, ShovelerCameraFreeDataFunction *freeData)
{
	camera->position = position;
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
	if(camera == NULL) {
		return;
	}

	shovelerUniformMapFree(camera->uniforms);
	camera->freeData(camera->data);
}
