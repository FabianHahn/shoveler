#include <assert.h> // assert
#include <math.h> // tanf
#include <stdlib.h> // malloc, free

#include "shoveler/camera/perspective.h"
#include "shoveler/types.h"

static void updateView(void *perspectiveCameraPointer);
static void freePerspectiveCamera(void *perspectiveCameraPointer);
static ShovelerCameraPerspective *getPerspectiveCamera(ShovelerCamera *camera);
static ShovelerMatrix computePerspectiveTransformation(float fovy, float ar, float znear, float zfar);
static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up);
static void controllerTiltCallback(ShovelerController *controller, ShovelerVector2 amount, void *userData);
static void controllerMoveCallback(ShovelerController *controller, ShovelerVector3 amount, void *userData);

ShovelerCamera *shovelerCameraPerspectiveCreate(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up, float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	ShovelerCameraPerspective *perspectiveCamera = malloc(sizeof(ShovelerCameraPerspective));
	shovelerCameraInit(&perspectiveCamera->camera, position, perspectiveCamera, updateView, freePerspectiveCamera);
	perspectiveCamera->up = shovelerVector3Normalize(up);
	perspectiveCamera->direction = shovelerVector3Normalize(direction);

	ShovelerVector3 right = shovelerVector3Normalize(shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->up));
	perspectiveCamera->upwards = shovelerVector3Cross(right, perspectiveCamera->direction);

	updateView(perspectiveCamera);
	perspectiveCamera->camera.projection = computePerspectiveTransformation(fieldOfViewY, aspectRatio, nearClippingPlane, farClippingPlane);
	perspectiveCamera->controller = NULL;

	return &perspectiveCamera->camera;
}

void shovelerCameraPerspectiveAttachController(ShovelerCamera *camera, ShovelerController *controller)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);

	controller->tilt = controllerTiltCallback;
	controller->tiltUserData = perspectiveCamera;
	controller->move = controllerMoveCallback;
	controller->moveUserData = perspectiveCamera;
	perspectiveCamera->controller = controller;
}

void shovelerCameraPerspectiveDetachController(ShovelerCamera *camera, ShovelerController *controller)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);

	assert(controller->tilt == controllerTiltCallback);
	assert(controller->tiltUserData == perspectiveCamera);
	assert(controller->move == controllerMoveCallback);
	assert(controller->moveUserData == perspectiveCamera);
	assert(perspectiveCamera->controller == controller);

	controller->tilt = NULL;
	controller->tiltUserData = NULL;
	controller->move = NULL;
	controller->moveUserData = NULL;
	perspectiveCamera->controller = NULL;
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
		shovelerCameraPerspectiveDetachController(&perspectiveCamera->camera, perspectiveCamera->controller);
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
	ShovelerMatrixGet(perspective, 0, 0) = f / apsectRatio;
	ShovelerMatrixGet(perspective, 1, 1) = f;
	ShovelerMatrixGet(perspective, 2, 2) = (nearClippingPlane + farClippingPlane) / (nearClippingPlane - farClippingPlane);
	ShovelerMatrixGet(perspective, 2, 3) = (2.0f * farClippingPlane * nearClippingPlane) / (nearClippingPlane - farClippingPlane);
	ShovelerMatrixGet(perspective, 3, 2) = -1.0f;

	return perspective;
}

static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 upwards)
{
	// Construct camera coordinate system basis
	ShovelerVector3 side = shovelerVector3Cross(direction, upwards);

	// Construct basis transform to camera coordinates
	ShovelerMatrix basis = shovelerMatrixIdentity;
	ShovelerMatrixGet(basis, 0, 0) = side.values[0];
	ShovelerMatrixGet(basis, 0, 1) = side.values[1];
	ShovelerMatrixGet(basis, 0, 2) = side.values[2];
	ShovelerMatrixGet(basis, 1, 0) = upwards.values[0];
	ShovelerMatrixGet(basis, 1, 1) = upwards.values[1];
	ShovelerMatrixGet(basis, 1, 2) = upwards.values[2];
	ShovelerMatrixGet(basis, 2, 0) = -direction.values[0];
	ShovelerMatrixGet(basis, 2, 1) = -direction.values[1];
	ShovelerMatrixGet(basis, 2, 2) = -direction.values[2];

	// Construct shift matrix to camera position
	ShovelerMatrix shift = shovelerMatrixIdentity;
	ShovelerMatrixGet(shift, 0, 3) = -position.values[0];
	ShovelerMatrixGet(shift, 1, 3) = -position.values[1];
	ShovelerMatrixGet(shift, 2, 3) = -position.values[2];

	// Create look into direction matrix
	return shovelerMatrixMultiply(basis, shift);
}

static void controllerTiltCallback(ShovelerController *controller, ShovelerVector2 amount, void *userData)
{
	ShovelerCameraPerspective *perspectiveCamera = userData;

	// Rotate camera in x direction
	ShovelerMatrix rotationX = shovelerMatrixCreateRotation(perspectiveCamera->up, amount.values[0]);
	ShovelerMatrix normalRotationX = shovelerMatrixTranspose(rotationX);

	perspectiveCamera->direction = shovelerVector3Normalize(shovelerMatrixMultiplyVector3(normalRotationX, perspectiveCamera->direction));

	ShovelerVector3 right = shovelerVector3Normalize(shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->up));
	perspectiveCamera->upwards = shovelerVector3Cross(right, perspectiveCamera->direction);

	// Rotate camera in y direction
	ShovelerMatrix rotationY = shovelerMatrixCreateRotation(right, amount.values[1]);
	ShovelerMatrix normalRotationY = shovelerMatrixTranspose(rotationY);

	ShovelerVector3 newDirection = shovelerVector3Normalize(shovelerMatrixMultiplyVector3(normalRotationY, perspectiveCamera->direction));
	ShovelerVector3 newUpwards = shovelerVector3Cross(right, newDirection);

	if(shovelerVector3Dot(perspectiveCamera->up, newUpwards) > 0.0f) { // only update if we're not flipping upside down
		perspectiveCamera->direction = newDirection;
		perspectiveCamera->upwards = newUpwards;
	}
}

static void controllerMoveCallback(ShovelerController *controller, ShovelerVector3 amount, void *userData)
{
	ShovelerCameraPerspective *perspectiveCamera = userData;

	ShovelerVector3 right = shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->upwards);

	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount.values[2], perspectiveCamera->direction);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount.values[0], right);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount.values[1], perspectiveCamera->upwards);
}
