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

	identityCamera->camera.frustum.nearBottomLeft = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, -1.0f, -1.0f));
	identityCamera->camera.frustum.nearBottomRight = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, -1.0f, -1.0f));
	identityCamera->camera.frustum.nearTopRight = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, 1.0f, -1.0f));
	identityCamera->camera.frustum.nearTopLeft = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, 1.0f, -1.0f));
	identityCamera->camera.frustum.farBottomLeft = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, -1.0f, 1.0f));
	identityCamera->camera.frustum.farBottomRight = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, -1.0f, 1.0f));
	identityCamera->camera.frustum.farTopRight = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(1.0f, 1.0f, 1.0f));
	identityCamera->camera.frustum.farTopLeft = shovelerVector3LinearCombination(1.0f, identityCamera->camera.position, 1.0f, shovelerVector3(-1.0f, 1.0f, 1.0f));
	identityCamera->camera.frustum.near = shovelerPlane(shovelerVector3(0.0f, 0.0f, -1.0f), -identityCamera->camera.position.values[0] + 1.0f);
	identityCamera->camera.frustum.far = shovelerPlane(shovelerVector3(0.0f, 0.0f, 1.0f), identityCamera->camera.position.values[0] + 1.0f);
	identityCamera->camera.frustum.left = shovelerPlane(shovelerVector3(-1.0f, 0.0f, 0.0f), -identityCamera->camera.position.values[1] + 1.0f);
	identityCamera->camera.frustum.bottom = shovelerPlane(shovelerVector3(0.0f, -1.0f, 0.0f), -identityCamera->camera.position.values[2] + 1.0f);
	identityCamera->camera.frustum.right = shovelerPlane(shovelerVector3(1.0f, 0.0f, 0.0f), identityCamera->camera.position.values[1] + 1.0f);
	identityCamera->camera.frustum.top = shovelerPlane(shovelerVector3(0.0f, 1.0f, 0.0f), identityCamera->camera.position.values[2] + 1.0f);
}

static void freeIdentityCamera(void *identityCameraPointer)
{
	free(identityCameraPointer);
}
