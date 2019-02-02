#ifndef SHOVELER_CAMERA_PERSPECTIVE_H
#define SHOVELER_CAMERA_PERSPECTIVE_H

#include <shoveler/camera.h>
#include <shoveler/controller.h>
#include <shoveler/projection.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerCamera camera;
	ShovelerVector3 direction;
	ShovelerVector3 upwards;
	ShovelerProjectionPerspective projection;
	ShovelerController *controller;
	ShovelerControllerTiltCallback *controllerTiltCallback;
	ShovelerControllerMoveCallback *controllerMoveCallback;
	ShovelerControllerAspectRatioChangeCallback *controllerAspectRatioChangeCallback;
	bool logPositionChanges;
} ShovelerCameraPerspective;

/** Creates a perspective camera from a reference frame and a perspective projection, copying both. */
ShovelerCamera *shovelerCameraPerspectiveCreate(const ShovelerReferenceFrame *frame, const ShovelerProjectionPerspective *projection);
void shovelerCameraPerspectiveAttachController(ShovelerCamera *camera, ShovelerController *controller);
ShovelerController *shovelerCameraPerspectiveGetController(ShovelerCamera *camera);
void shovelerCameraPerspectiveDetachController(ShovelerCamera *camera);

#endif
