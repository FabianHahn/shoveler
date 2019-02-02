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
static ShovelerMatrix computePerspectiveTransformation(float fovy, float ar, float znear, float zfar);
static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up);
static void tiltController(ShovelerController *controller, ShovelerVector3 direction, ShovelerVector3 upwards, void *userData);
static void moveController(ShovelerController *controller, ShovelerVector3 position, void *userData);
static void aspectRatioChangeController(ShovelerController *controller, float aspectRatio, void *userData);

ShovelerCamera *shovelerCameraPerspectiveCreate(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 upwards, float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	ShovelerCameraPerspective *perspectiveCamera = malloc(sizeof(ShovelerCameraPerspective));
	shovelerCameraInit(&perspectiveCamera->camera, position, perspectiveCamera, updateView, freePerspectiveCamera);
	perspectiveCamera->direction = shovelerVector3Normalize(direction);
	perspectiveCamera->upwards = shovelerVector3Normalize(upwards);

	updateView(perspectiveCamera);

	perspectiveCamera->fieldOfViewY = fieldOfViewY;
	perspectiveCamera->aspectRatio = aspectRatio;
	perspectiveCamera->nearClippingPlane = nearClippingPlane;
	perspectiveCamera->farClippingPlane = farClippingPlane;
	perspectiveCamera->camera.projection = computePerspectiveTransformation(perspectiveCamera->fieldOfViewY, perspectiveCamera->aspectRatio, perspectiveCamera->nearClippingPlane, perspectiveCamera->farClippingPlane);

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
	perspectiveCamera->camera.view = computeLookIntoDirectionTransformation(perspectiveCamera->camera.position, perspectiveCamera->direction, perspectiveCamera->upwards);
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

static ShovelerMatrix computePerspectiveTransformation(float fieldOfViewY, float apsectRatio, float nearClippingPlane, float farClippingPlane)
{
	ShovelerMatrix perspective = shovelerMatrixZero;

	float f = 1.0f / tanf(fieldOfViewY / 2.0f);
	shovelerMatrixGet(perspective, 0, 0) = f / apsectRatio;
	shovelerMatrixGet(perspective, 1, 1) = f;
	shovelerMatrixGet(perspective, 2, 2) = (nearClippingPlane + farClippingPlane) / (nearClippingPlane - farClippingPlane);
	shovelerMatrixGet(perspective, 2, 3) = (2.0f * farClippingPlane * nearClippingPlane) / (nearClippingPlane - farClippingPlane);
	shovelerMatrixGet(perspective, 3, 2) = -1.0f;

	return perspective;
}

static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 upwards)
{
	// Construct camera coordinate system basis
	ShovelerVector3 side = shovelerVector3Cross(direction, upwards);

	// Construct basis transform to camera coordinates
	ShovelerMatrix basis = shovelerMatrixIdentity;
	shovelerMatrixGet(basis, 0, 0) = side.values[0];
	shovelerMatrixGet(basis, 0, 1) = side.values[1];
	shovelerMatrixGet(basis, 0, 2) = side.values[2];
	shovelerMatrixGet(basis, 1, 0) = upwards.values[0];
	shovelerMatrixGet(basis, 1, 1) = upwards.values[1];
	shovelerMatrixGet(basis, 1, 2) = upwards.values[2];
	shovelerMatrixGet(basis, 2, 0) = -direction.values[0];
	shovelerMatrixGet(basis, 2, 1) = -direction.values[1];
	shovelerMatrixGet(basis, 2, 2) = -direction.values[2];

	// Construct shift matrix to camera position
	ShovelerMatrix shift = shovelerMatrixIdentity;
	shovelerMatrixGet(shift, 0, 3) = -position.values[0];
	shovelerMatrixGet(shift, 1, 3) = -position.values[1];
	shovelerMatrixGet(shift, 2, 3) = -position.values[2];

	// Create look into direction matrix
	return shovelerMatrixMultiply(basis, shift);
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

	if (aspectRatio != perspectiveCamera->aspectRatio) {
		perspectiveCamera->aspectRatio = aspectRatio;
		perspectiveCamera->camera.projection = computePerspectiveTransformation(perspectiveCamera->fieldOfViewY, perspectiveCamera->aspectRatio, perspectiveCamera->nearClippingPlane, perspectiveCamera->farClippingPlane);
	}
}
