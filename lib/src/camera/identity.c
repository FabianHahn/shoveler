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

	ShovelerMatrixGet(identityCamera->camera.view, 0, 3) = -identityCamera->camera.position.values[0];
	ShovelerMatrixGet(identityCamera->camera.view, 1, 3) = -identityCamera->camera.position.values[1];
	ShovelerMatrixGet(identityCamera->camera.view, 2, 3) = -identityCamera->camera.position.values[2];
}

static void freeIdentityCamera(void *identityCameraPointer)
{
	free(identityCameraPointer);
}
