#ifndef SHOVELER_VIEW_LIGHT_H
#define SHOVELER_VIEW_LIGHT_H

#include <glad/glad.h>

#include <shoveler/types.h>
#include <shoveler/view.h>

typedef enum {
	SHOVELER_VIEW_LIGHT_TYPE_SPOT,
	SHOVELER_VIEW_LIGHT_TYPE_POINT
} ShovelerViewLightType;

typedef struct {
	ShovelerViewLightType type;
	int width;
	int height;
	GLsizei samples;
	float ambientFactor;
	float exponentialFactor;
	ShovelerVector3 color;
} ShovelerViewLightConfiguration;

static const char *shovelerViewLightComponentName = "light";

bool shovelerViewEntityAddLight(ShovelerViewEntity *entity, ShovelerViewLightConfiguration configuration);
const ShovelerViewLightConfiguration *shovelerViewEntityGetLightConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateLight(ShovelerViewEntity *entity, ShovelerViewLightConfiguration configuration);
bool shovelerViewEntityRemoveLight(ShovelerViewEntity *entity);

#endif
