#ifndef SHOVELER_CAMERA_PERSPECTIVE_H
#define SHOVELER_CAMERA_PERSPECTIVE_H

#include <shoveler/camera.h>
#include <shoveler/controller.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerCamera camera;
	ShovelerVector3 direction;
	ShovelerVector3 upwards;
	float fieldOfViewY;
	float aspectRatio;
	float nearClippingPlane;
	float farClippingPlane;
	ShovelerController *controller;
	ShovelerControllerTiltCallback *controllerTiltCallback;
	ShovelerControllerMoveCallback *controllerMoveCallback;
	ShovelerControllerAspectRatioChangeCallback *controllerAspectRatioChangeCallback;
} ShovelerCameraPerspective;

ShovelerCamera *shovelerCameraPerspectiveCreate(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 upwards, float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane);
void shovelerCameraPerspectiveAttachController(ShovelerCamera *camera, ShovelerController *controller);
ShovelerController * shovelerCameraPerspectiveGetController(ShovelerCamera *camera);
void shovelerCameraPerspectiveDetachController(ShovelerCamera *camera);

#endif
