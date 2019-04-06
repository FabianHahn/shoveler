#include <stdlib.h> // malloc, free

#include "shoveler/camera.h"
#include "shoveler/shader_cache.h"

void shovelerCameraInit(ShovelerCamera *camera, ShovelerShaderCache *shaderCache, ShovelerVector3 position, void *data, ShovelerCameraUpdateViewFunction *updateView, ShovelerCameraFreeDataFunction *freeData)
{
	camera->shaderCache = shaderCache;
	camera->position = position;
	camera->frustum.nearBottomLeftVertex = shovelerVector3(-1.0f, -1.0f, -1.0f);
	camera->frustum.nearBottomRightVertex = shovelerVector3(1.0f, -1.0f, -1.0f);
	camera->frustum.nearTopRightVertex = shovelerVector3(1.0f, 1.0f, -1.0f);
	camera->frustum.nearTopLeftVertex = shovelerVector3(-1.0f, 1.0f, -1.0f);
	camera->frustum.farBottomLeftVertex = shovelerVector3(-1.0f, -1.0f, 1.0f);
	camera->frustum.farBottomRightVertex = shovelerVector3(1.0f, -1.0f, 1.0f);
	camera->frustum.farTopRightVertex = shovelerVector3(1.0f, 1.0f, 1.0f);
	camera->frustum.farTopLeftVertex = shovelerVector3(-1.0f, 1.0f, 1.0f);
	camera->frustum.nearPlane = shovelerPlane(shovelerVector3(0.0f, 0.0f, -1.0f), 1.0f);
	camera->frustum.farPlane = shovelerPlane(shovelerVector3(0.0f, 0.0f, 1.0f), 1.0f);
	camera->frustum.leftPlane = shovelerPlane(shovelerVector3(-1.0f, 0.0f, 0.0f), 1.0f);
	camera->frustum.bottomPlane = shovelerPlane(shovelerVector3(0.0f, -1.0f, 0.0f), 1.0f);
	camera->frustum.rightPlane = shovelerPlane(shovelerVector3(1.0f, 0.0f, 0.0f), 1.0f);
	camera->frustum.topPlane = shovelerPlane(shovelerVector3(0.0f, 1.0f, 0.0f), 1.0f);
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

	shovelerShaderCacheInvalidateCamera(camera->shaderCache, camera);

	shovelerUniformMapFree(camera->uniforms);
	camera->freeData(camera->data);
}
