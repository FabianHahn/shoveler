#include <stdlib.h> // malloc, free

#include "shoveler/camera/perspective.h"
#include "shoveler/types.h"

typedef struct {
	ShovelerCamera camera;
} ShovelerCameraIdentity;

static void updateView(void *identityCameraPointer);
static void freeIdentityCamera(void *identityCameraPointer);

ShovelerCamera *shovelerCameraIdentityCreate()
{
	ShovelerCameraIdentity *identityCamera = malloc(sizeof(ShovelerCameraIdentity));
	shovelerCameraInit(&identityCamera->camera, (ShovelerVector3){0, 0, 0}, identityCamera, updateView, freeIdentityCamera);
	return &identityCamera->camera;
}

static void updateView(void *identityCameraPointer)
{
	ShovelerCameraIdentity *identityCamera = identityCameraPointer;

	shovelerMatrixGet(identityCamera->camera.view, 0, 3) = -identityCamera->camera.position.values[0];
	shovelerMatrixGet(identityCamera->camera.view, 1, 3) = -identityCamera->camera.position.values[1];
	shovelerMatrixGet(identityCamera->camera.view, 2, 3) = -identityCamera->camera.position.values[2];

	identityCamera->camera.frustum.nearBottomLeftVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, -1.0f, -1.0f));
	identityCamera->camera.frustum.nearBottomRightVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, -1.0f, -1.0f));
	identityCamera->camera.frustum.nearTopRightVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, 1.0f, -1.0f));
	identityCamera->camera.frustum.nearTopLeftVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, 1.0f, -1.0f));
	identityCamera->camera.frustum.farBottomLeftVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, -1.0f, 1.0f));
	identityCamera->camera.frustum.farBottomRightVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, -1.0f, 1.0f));
	identityCamera->camera.frustum.farTopRightVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, 1.0f, 1.0f));
	identityCamera->camera.frustum.farTopLeftVertex = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, 1.0f, 1.0f));
	identityCamera->camera.frustum.nearPlane = shovelerPlane(shovelerVector3(0.0f, 0.0f, -1.0f), -identityCamera->camera.position.values[0] + 1.0f);
	identityCamera->camera.frustum.farPlane = shovelerPlane(shovelerVector3(0.0f, 0.0f, 1.0f), identityCamera->camera.position.values[0] + 1.0f);
	identityCamera->camera.frustum.leftPlane = shovelerPlane(shovelerVector3(-1.0f, 0.0f, 0.0f), -identityCamera->camera.position.values[1] + 1.0f);
	identityCamera->camera.frustum.bottomPlane = shovelerPlane(shovelerVector3(0.0f, -1.0f, 0.0f), -identityCamera->camera.position.values[2] + 1.0f);
	identityCamera->camera.frustum.rightPlane = shovelerPlane(shovelerVector3(1.0f, 0.0f, 0.0f), identityCamera->camera.position.values[1] + 1.0f);
	identityCamera->camera.frustum.topPlane = shovelerPlane(shovelerVector3(0.0f, 1.0f, 0.0f), identityCamera->camera.position.values[2] + 1.0f);
}

static void freeIdentityCamera(void *identityCameraPointer)
{
	free(identityCameraPointer);
}
