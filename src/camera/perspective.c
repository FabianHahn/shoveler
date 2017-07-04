#include <math.h> // tanf
#include <stdlib.h> // malloc, free

#include "camera/perspective.h"
#include "types.h"

static void freePerspectiveCamera(void *perspectiveCameraPointer);
static ShovelerCameraPerspective *getPerspectiveCamera(ShovelerCamera *camera);
static ShovelerMatrix computePerspectiveTransformation(float fovy, float ar, float znear, float zfar);
static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up);

ShovelerCamera *shovelerCameraPerspectiveCreate(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up, float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	ShovelerCameraPerspective *perspectiveCamera = malloc(sizeof(ShovelerCameraPerspective));
	shovelerCameraInit(&perspectiveCamera->camera, position, perspectiveCamera, freePerspectiveCamera);
	perspectiveCamera->up = shovelerVector3Normalize(up);
	perspectiveCamera->direction = shovelerVector3Normalize(direction);
	shovelerCameraPerspectiveUpdateView(&perspectiveCamera->camera);
	perspectiveCamera->camera.projection = computePerspectiveTransformation(fieldOfViewY, aspectRatio, nearClippingPlane, farClippingPlane);

	return &perspectiveCamera->camera;
}

void shovelerCameraPerspectiveUpdateView(ShovelerCamera *camera)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	perspectiveCamera->camera.view = computeLookIntoDirectionTransformation(perspectiveCamera->camera.position, perspectiveCamera->direction, perspectiveCamera->up);
}

void shovelerCameraPerspectiveMoveForward(ShovelerCamera *camera, float amount)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount, perspectiveCamera->direction);
}

void shovelerCameraPerspectiveMoveRight(ShovelerCamera *camera, float amount)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	ShovelerVector3 right = shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->up);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount, right);
}

void shovelerCameraPerspectiveMoveUp(ShovelerCamera *camera, float amount)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	ShovelerVector3 right = shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->up);
	ShovelerVector3 upwards = shovelerVector3Cross(right, perspectiveCamera->direction);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount, upwards);
}

void shovelerCameraPerspectiveTiltUp(ShovelerCamera *camera, float amount)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);

	ShovelerVector3 right = shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->up);

	// Rotate camera direction
	ShovelerMatrix rotation = shovelerMatrixCreateRotation(right, amount);
	ShovelerMatrix normalRotation = shovelerMatrixTranspose(rotation);

	ShovelerVector3 newDirection = shovelerVector3Normalize(shovelerMatrixMultiplyVector3(normalRotation, perspectiveCamera->direction));
	ShovelerVector3 newUp = shovelerVector3Cross(right, newDirection);

	if(newUp.values[1] >= 0.0f) { // only update if we're not flipping upside down
		perspectiveCamera->direction = newDirection;
		perspectiveCamera->up = newUp;
	}
}

void shovelerCameraPerspectiveTiltRight(ShovelerCamera *camera, float amount)
{
	ShovelerCameraPerspective *perspectiveCamera = getPerspectiveCamera(camera);

	// Rotate camera direction
	ShovelerMatrix rotation = shovelerMatrixCreateRotation(perspectiveCamera->up, amount);
	ShovelerMatrix normalRotation = shovelerMatrixTranspose(rotation);

	ShovelerVector3 newDirection = shovelerVector3Normalize(shovelerMatrixMultiplyVector3(normalRotation, perspectiveCamera->direction));
	ShovelerVector3 newRight = shovelerVector3Cross(newDirection, perspectiveCamera->up);
	newRight.values[1] = 0.0f;

	ShovelerVector3 newUp = shovelerVector3Normalize(shovelerVector3Cross(newRight, newDirection));

	perspectiveCamera->direction = newDirection;
	perspectiveCamera->up = newUp;
}

static void freePerspectiveCamera(void *perspectiveCameraPointer)
{
	free(perspectiveCameraPointer);
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

static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up)
{
	// Construct camera coordinate system basis
	ShovelerVector3 side = shovelerVector3Cross(direction, up);
	ShovelerVector3 upwards = shovelerVector3Cross(side, direction);

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
