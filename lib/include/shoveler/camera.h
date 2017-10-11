#ifndef SHOVELER_CAMERA_H
#define SHOVELER_CAMERA_H

#include <shoveler/types.h>
#include <shoveler/uniform_map.h>

struct ShovelerCameraStruct;

typedef void (ShovelerCameraUpdateViewFunction)(void *data);
typedef void (ShovelerCameraFreeDataFunction)(void *data);

typedef struct ShovelerCameraStruct {
	ShovelerVector3 position;
	ShovelerMatrix view;
	ShovelerMatrix projection;
	ShovelerUniformMap *uniforms;
	void *data;
	ShovelerCameraUpdateViewFunction *updateView;
	ShovelerCameraFreeDataFunction *freeData;
} ShovelerCamera;

void shovelerCameraInit(ShovelerCamera *camera, ShovelerVector3 position, void *data, ShovelerCameraUpdateViewFunction *updateView, ShovelerCameraFreeDataFunction *freeData);
void shovelerCameraFree(ShovelerCamera *camera);

static inline void shovelerCameraUpdateView(ShovelerCamera *camera)
{
	camera->updateView(camera->data);
}


#endif
