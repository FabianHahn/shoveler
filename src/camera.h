#ifndef SHOVELER_CAMERA_H
#define SHOVELER_CAMERA_H

#include "types.h"
#include "uniform_map.h"

struct ShovelerCameraStruct;

typedef void (ShovelerCameraFreeDataFunction)(void *data);

typedef struct ShovelerCameraStruct {
	ShovelerVector3 position;
	ShovelerMatrix view;
	ShovelerMatrix projection;
	ShovelerUniformMap *uniforms;
	void *data;
	ShovelerCameraFreeDataFunction *freeData;
} ShovelerCamera;

void shovelerCameraInit(ShovelerCamera *camera, ShovelerVector3 position, void *data, ShovelerCameraFreeDataFunction *freeData);
void shovelerCameraFree(ShovelerCamera *camera);

#endif
