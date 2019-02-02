#include <assert.h> // assert
#include <math.h> // tanf
#include <memory.h> // memcmp
#include <stdlib.h> // malloc, free

#include "shoveler/camera/perspective.h"
#include "shoveler/log.h"
#include "shoveler/types.h"

static void updateView(void *perspectiveCameraPointer);
static void freePerspectiveCamera(void *perspectiveCameraPointer);
static ShovelerCameraPerspective *getPerspectiveCamera(ShovelerCamera *camera);
static void tiltController(ShovelerController *controller, ShovelerVector3 direction, ShovelerVector3 upwards, void *userData);
static void moveController(ShovelerController *controller, ShovelerVector3 position, void *userData);
static void aspectRatioChangeController(ShovelerController *controller, float aspectRatio, void *userData);

ShovelerCamera *shovelerCameraPerspectiveCreate(const ShovelerReferenceFrame *frame, const ShovelerProjectionPerspective *projection)
{
	ShovelerCameraPerspective *perspectiveCamera = malloc(sizeof(ShovelerCameraPerspective));
	shovelerCameraInit(&perspectiveCamera->camera, frame->position, perspectiveCamera, updateView, freePerspectiveCamera);
	perspectiveCamera->direction = shovelerVector3Normalize(frame->direction);
	perspectiveCamera->upwards = shovelerVector3Normalize(frame->up);

	perspectiveCamera->projection = *projection;
	shovelerProjectionPerspectiveComputeTransformation(&perspectiveCamera->projection, &perspectiveCamera->camera.projection);

	updateView(perspectiveCamera);

	perspectiveCamera->controller = NULL;
	perspectiveCamera->controllerTiltCallback = NULL;
	perspectiveCamera->controllerMoveCallback = NULL;
	perspectiveCamera->controllerAspectRatioChangeCallback = NULL;

	perspectiveCamera->logPositionChanges = false;

	return &perspectiveCamera->camera;
}

void shovelerCameraPerspectiveAttachController(ShovelerCamera *camera, ShovelerController *controller)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	assert(perspectiveCamera->controller == NULL);

	perspectiveCamera->controller = controller;
	perspectiveCamera->controllerTiltCallback = shovelerControllerAddTiltCallback(controller, tiltController, perspectiveCamera);
	perspectiveCamera->controllerMoveCallback = shovelerControllerAddMoveCallback(controller, moveController, perspectiveCamera);
	perspectiveCamera->controllerAspectRatioChangeCallback = shovelerControllerAddAspectRatioChangeCallback(controller, aspectRatioChangeController, perspectiveCamera);
}

ShovelerController *shovelerCameraPerspectiveGetController(ShovelerCamera *camera)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	return perspectiveCamera->controller;
}

void shovelerCameraPerspectiveDetachController(ShovelerCamera *camera)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	assert(perspectiveCamera->controller != NULL);

	shovelerControllerRemoveTiltCallback(perspectiveCamera->controller, perspectiveCamera->controllerTiltCallback);
	shovelerControllerRemoveMoveCallback(perspectiveCamera->controller, perspectiveCamera->controllerMoveCallback);
	shovelerControllerRemoveAspectRatioChangeCallback(perspectiveCamera->controller, perspectiveCamera->controllerAspectRatioChangeCallback);

	perspectiveCamera->controller = NULL;
	perspectiveCamera->controllerTiltCallback = NULL;
	perspectiveCamera->controllerMoveCallback = NULL;
}

static void updateView(void *perspectiveCameraPointer)
{
	ShovelerCameraPerspective *perspectiveCamera = perspectiveCameraPointer;

	ShovelerReferenceFrame frame;
	frame.position = perspectiveCamera->camera.position;
	frame.direction = perspectiveCamera->direction;
	frame.up = perspectiveCamera->upwards;
	shovelerMatrixCreateLookIntoDirectionTransformation(&frame, &perspectiveCamera->camera.view);

	shovelerProjectionPerspectiveComputeFrustum(&perspectiveCamera->projection, &frame, &perspectiveCamera->camera.frustum);
}

static void freePerspectiveCamera(void *perspectiveCameraPointer)
{
	ShovelerCameraPerspective *perspectiveCamera = perspectiveCameraPointer;

	if(perspectiveCamera->controller != NULL) {
		shovelerCameraPerspectiveDetachController(&perspectiveCamera->camera);
	}

	free(perspectiveCamera);
}

static ShovelerCameraPerspective *getPerspectiveCamera(ShovelerCamera *camera)
{
	return camera->data;
}

static void tiltController(ShovelerController *controller, ShovelerVector3 direction, ShovelerVector3 upwards, void *userData)
{
	ShovelerCameraPerspective *perspectiveCamera = userData;

	perspectiveCamera->direction = direction;
	perspectiveCamera->upwards = upwards;
}

static void moveController(ShovelerController *controller, ShovelerVector3 position, void *userData)
{
	ShovelerCameraPerspective *perspectiveCamera = userData;

	if(perspectiveCamera->logPositionChanges && memcmp(&position, &perspectiveCamera->camera.position, sizeof(ShovelerVector3)) != 0) {
		shovelerLogInfo("Camera at (%.2f, %.2f, %.2f).", perspectiveCamera->camera.position.values[0], perspectiveCamera->camera.position.values[1], perspectiveCamera->camera.position.values[2]);
	}

	perspectiveCamera->camera.position = position;
}

static void aspectRatioChangeController(ShovelerController *controller, float aspectRatio, void *userData)
{
	ShovelerCameraPerspective *perspectiveCamera = userData;

	if (aspectRatio != perspectiveCamera->projection.aspectRatio) {
		perspectiveCamera->projection.aspectRatio = aspectRatio;
		shovelerProjectionPerspectiveComputeTransformation(&perspectiveCamera->projection, &perspectiveCamera->camera.projection);
	}
}
