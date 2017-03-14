#ifndef SHOVELER_CAMERA_PERSPECTIVE_H
#define SHOVELER_CAMERA_PERSPECTIVE_H

#include "camera.h"
#include "types.h"

typedef struct {
	ShovelerCamera camera;
	ShovelerVector3 up;
	ShovelerVector3 direction;
	ShovelerMatrix perspective;
} ShovelerCameraPerspective;

ShovelerCamera *shovelerCameraPerspectiveCreate(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up, float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane);
void shovelerCameraPerspectiveUpdateTransformation(ShovelerCamera *camera);
void shovelerCameraPerspectiveMoveForward(ShovelerCamera *camera, float amount);
void shovelerCameraPerspectiveMoveRight(ShovelerCamera *camera, float amount);
void shovelerCameraPerspectiveMoveUp(ShovelerCamera *camera, float amount);
void shovelerCameraPerspectiveTiltUp(ShovelerCamera *camera, float amount);
void shovelerCameraPerspectiveTiltRight(ShovelerCamera *camera, float amount);

#endif
