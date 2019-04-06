#ifndef SHOVELER_CAMERA_H
#define SHOVELER_CAMERA_H

#include <shoveler/frustum.h>
#include <shoveler/types.h>
#include <shoveler/uniform_map.h>

struct ShovelerCameraStruct;
struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

typedef void (ShovelerCameraUpdateViewFunction)(void *data);
typedef void (ShovelerCameraFreeDataFunction)(void *data);

typedef struct ShovelerCameraStruct {
	struct ShovelerShaderCacheStruct *shaderCache;
	ShovelerVector3 position;
	ShovelerFrustum frustum;
	ShovelerMatrix view;
	ShovelerMatrix projection;
	ShovelerUniformMap *uniforms;
	void *data;
	ShovelerCameraUpdateViewFunction *updateView;
	ShovelerCameraFreeDataFunction *freeData;
} ShovelerCamera;

void shovelerCameraInit(ShovelerCamera *camera, struct ShovelerShaderCacheStruct *shaderCache, ShovelerVector3 position, void *data, ShovelerCameraUpdateViewFunction *updateView, ShovelerCameraFreeDataFunction *freeData);
void shovelerCameraFree(ShovelerCamera *camera);

/** Asks the camera implementation to update its view matrix and frustum. */
static inline void shovelerCameraUpdateView(ShovelerCamera *camera)
{
	camera->updateView(camera->data);
}


#endif
