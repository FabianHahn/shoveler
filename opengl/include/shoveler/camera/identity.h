#ifndef SHOVELER_CAMERA_IDENTITY_H
#define SHOVELER_CAMERA_IDENTITY_H

#include <shoveler/camera.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerCamera camera;
} ShovelerCameraIdentity;

ShovelerCamera *shovelerCameraIdentityCreate();
void shovelerCameraIdentityFree(ShovelerCamera *camera);

#endif
