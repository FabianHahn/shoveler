#ifndef SHOVELER_CAMERAS_PERSPECTIVE_H
#define SHOVELER_CAMERAS_PERSPECTIVE_H

#include "camera.h"
#include "types.h"

typedef struct {
	ShovelerCamera camera;
	ShovelerVector3 up;
	ShovelerVector3 direction;
	ShovelerMatrix perspective;
} ShovelerCamerasPerspective;

ShovelerCamera *shovelerCamerasPerspectiveCreate(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up, float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane);
void shovelerCamerasPerspectiveUpdateTransformation(ShovelerCamera *camera);
void shovelerCamerasPerspectiveMoveForward(ShovelerCamera *camera, float amount);
void shovelerCamerasPerspectiveMoveRight(ShovelerCamera *camera, float amount);
void shovelerCamerasPerspectiveMoveUp(ShovelerCamera *camera, float amount);
void shovelerCamerasPerspectiveTiltUp(ShovelerCamera *camera, float amount);
void shovelerCamerasPerspectiveTiltRight(ShovelerCamera *camera, float amount);
void shovelerCamerasPerspectiveFree(ShovelerCamera *camera);

#endif
