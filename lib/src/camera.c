#include <stdlib.h> // malloc, free

#include "shoveler/camera.h"

void shovelerCameraInit(ShovelerCamera *camera, ShovelerVector3 position, void *data, ShovelerCameraFreeDataFunction *freeData)
{
	camera->position = position;
	camera->view = shovelerMatrixIdentity;
	camera->projection = shovelerMatrixIdentity;
	camera->uniforms = shovelerUniformMapCreate();
	camera->data = data;
	camera->freeData = freeData;

	ShovelerUniform *viewUniform = shovelerUniformCreateMatrixPointer(&camera->view);
	shovelerUniformMapInsert(camera->uniforms, "view", viewUniform);
	ShovelerUniform *projectionUniform = shovelerUniformCreateMatrixPointer(&camera->projection);
	shovelerUniformMapInsert(camera->uniforms, "projection", projectionUniform);
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
