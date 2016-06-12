#include <math.h> // tanf
#include <stdlib.h> // malloc, free

#include "cameras/perspective.h"
#include "types.h"

static void updateTransformation(ShovelerCamerasPerspective *perspectiveCamera);
static ShovelerMatrix computePerspectiveTransformation(float fovy, float ar, float znear, float zfar);
static ShovelerMatrix computeLookIntoDirectionTransformation(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up);

ShovelerCamerasPerspective *shovelerCamerasPerspectiveCreate(float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	ShovelerCamerasPerspective *perspectiveCamera = malloc(sizeof(ShovelerCamerasPerspective));
	perspectiveCamera->camera = shovelerCameraCreate();
	perspectiveCamera->up = (ShovelerVector3){{0, 1, 0}};
	perspectiveCamera->direction = (ShovelerVector3){{0, 0, 1}};
	perspectiveCamera->perspective = computePerspectiveTransformation(fieldOfViewY, aspectRatio, nearClippingPlane, farClippingPlane);
	updateTransformation(perspectiveCamera);

	return perspectiveCamera;
}

void shovelerCamerasPerspectiveFree(ShovelerCamerasPerspective *perspectiveCamera)
{
	shovelerCameraFree(perspectiveCamera->camera);
	free(perspectiveCamera);
}

static void updateTransformation(ShovelerCamerasPerspective *perspectiveCamera)
{
	ShovelerMatrix lookIntoDirection = computeLookIntoDirectionTransformation(perspectiveCamera->camera->position, perspectiveCamera->direction, perspectiveCamera->up);
	perspectiveCamera->camera->transformation = shovelerMatrixMultiply(perspectiveCamera->perspective, lookIntoDirection);
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
