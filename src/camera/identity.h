#ifndef SHOVELER_CAMERA_IDENTITY_H
#define SHOVELER_CAMERA_IDENTITY_H

#include "camera.h"
#include "types.h"

typedef struct {
	ShovelerCamera camera;
} ShovelerCameraIdentity;

ShovelerCamera *shovelerCameraIdentityCreate();
void shovelerCameraIdentityFree(ShovelerCamera *camera);

#endif
