#include <stdlib.h> // malloc, free

#include "shoveler/camera.h"

void shovelerCameraInit(ShovelerCamera *camera, ShovelerVector3 position, void *data, ShovelerCameraUpdateViewFunction *updateView, ShovelerCameraFreeDataFunction *freeData)
{
	camera->position = position;
	camera->frustum.nearBottomLeft = shovelerVector3(-1.0f, -1.0f, -1.0f);
	camera->frustum.nearBottomRight = shovelerVector3(1.0f, -1.0f, -1.0f);
	camera->frustum.nearTopRight = shovelerVector3(1.0f, 1.0f, -1.0f);
	camera->frustum.nearTopLeft = shovelerVector3(-1.0f, 1.0f, -1.0f);
	camera->frustum.farBottomLeft = shovelerVector3(-1.0f, -1.0f, 1.0f);
	camera->frustum.farBottomRight = shovelerVector3(1.0f, -1.0f, 1.0f);
	camera->frustum.farTopRight = shovelerVector3(1.0f, 1.0f, 1.0f);
	camera->frustum.farTopLeft = shovelerVector3(-1.0f, 1.0f, 1.0f);
	camera->frustum.near = shovelerPlane(shovelerVector3(0.0f, 0.0f, -1.0f), 1.0f);
	camera->frustum.far = shovelerPlane(shovelerVector3(0.0f, 0.0f, 1.0f), 1.0f);
	camera->frustum.left = shovelerPlane(shovelerVector3(-1.0f, 0.0f, 0.0f), 1.0f);
	camera->frustum.bottom = shovelerPlane(shovelerVector3(0.0f, -1.0f, 0.0f), 1.0f);
	camera->frustum.right = shovelerPlane(shovelerVector3(1.0f, 0.0f, 0.0f), 1.0f);
	camera->frustum.top = shovelerPlane(shovelerVector3(0.0f, 1.0f, 0.0f), 1.0f);
	camera->view = shovelerMatrixIdentity;
	camera->projection = shovelerMatrixIdentity;
	camera->uniforms = shovelerUniformMapCreate();
	camera->data = data;
	camera->updateView = updateView;
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
