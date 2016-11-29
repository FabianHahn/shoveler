#include <math.h> // tanf
#include <stdlib.h> // malloc, free

#include "cameras/perspective.h"
#include "types.h"

static void freePerspectiveCamera(void *perspectiveCameraPointer);
static ShovelerCamerasPerspective *getPerspectiveCamera(ShovelerCamera *camera);
static void updateTransformation(ShovelerCamerasPerspective *perspectiveCamera);
static ShovelerMatrix computePerspectiveTransformation(float fovy, float ar, float znear, float zfar);
static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up);

ShovelerCamera *shovelerCamerasPerspectiveCreate(float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	ShovelerCamerasPerspective *perspectiveCamera = malloc(sizeof(ShovelerCamerasPerspective));
	shovelerCameraInit(&perspectiveCamera->camera, perspectiveCamera, freePerspectiveCamera);
	perspectiveCamera->up = (ShovelerVector3){{0, 1, 0}};
	perspectiveCamera->direction = (ShovelerVector3){{0, 0, 1}};
	perspectiveCamera->perspective = computePerspectiveTransformation(fieldOfViewY, aspectRatio, nearClippingPlane, farClippingPlane);
	shovelerCamerasPerspectiveUpdateTransformation(&perspectiveCamera->camera);

	return &perspectiveCamera->camera;
}

void shovelerCamerasPerspectiveUpdateTransformation(ShovelerCamera *camera)
{
	ShovelerCamerasPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	ShovelerMatrix lookIntoDirection = computeLookIntoDirectionTransformation(perspectiveCamera->camera.position, perspectiveCamera->direction, perspectiveCamera->up);
	perspectiveCamera->camera.transformation = shovelerMatrixMultiply(perspectiveCamera->perspective, lookIntoDirection);
}

void shovelerCamerasPerspectiveMoveForward(ShovelerCamera *camera, float amount)
{
	ShovelerCamerasPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount, perspectiveCamera->direction);
}

void shovelerCamerasPerspectiveMoveRight(ShovelerCamera *camera, float amount)
{
	ShovelerCamerasPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	ShovelerVector3 right = shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->up);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount, right);
}

void shovelerCamerasPerspectiveMoveUp(ShovelerCamera *camera, float amount)
{
	ShovelerCamerasPerspective *perspectiveCamera = getPerspectiveCamera(camera);
	ShovelerVector3 right = shovelerVector3Cross(perspectiveCamera->direction, perspectiveCamera->up);
	ShovelerVector3 upwards = shovelerVector3Cross(right, perspectiveCamera->direction);
	perspectiveCamera->camera.position = shovelerVector3LinearCombination(1.0, perspectiveCamera->camera.position, amount, upwards);
}

void shovelerCamerasPerspectiveTiltUp(ShovelerCamera *camera, float amount)
{
	ShovelerCamerasPerspective *perspectiveCamera = getPerspectiveCamera(camera);

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

void shovelerCamerasPerspectiveTiltRight(ShovelerCamera *camera, float amount)
{
	ShovelerCamerasPerspective *perspectiveCamera = getPerspectiveCamera(camera);

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

static ShovelerCamerasPerspective *getPerspectiveCamera(ShovelerCamera *camera)
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
