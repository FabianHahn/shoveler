#ifndef SHOVELER_CAMERA_IDENTITY_H
#define SHOVELER_CAMERA_IDENTITY_H

#include <shoveler/camera.h>
#include <shoveler/types.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

typedef struct {
	ShovelerCamera camera;
} ShovelerCameraIdentity;

ShovelerCamera *shovelerCameraIdentityCreate(struct ShovelerShaderCacheStruct *shaderCache);
void shovelerCameraIdentityFree(ShovelerCamera *camera);

#endif
