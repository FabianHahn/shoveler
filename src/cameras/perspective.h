#ifndef SHOVELER_CAMERAS_PERSPECTIVE_H
#define SHOVELER_CAMERAS_PERSPECTIVE_H

#include "camera.h"

typedef struct {
	ShovelerCamera *camera;
	ShovelerVector3 up;
	ShovelerVector3 direction;
	ShovelerMatrix perspective;
} ShovelerCamerasPerspective;

ShovelerCamerasPerspective *shovelerCamerasPerspectiveCreate(float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane);
void shovelerCamerasPerspectiveFree(ShovelerCamerasPerspective *perspectiveCamera);

#endif
