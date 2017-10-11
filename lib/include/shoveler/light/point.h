#ifndef SHOVELER_LIGHT_POINT_H
#define SHOVELER_LIGHT_POINT_H

#include <shoveler/light/spot.h>

typedef struct {
	ShovelerLight light;
	ShovelerLightSpotShared *shared;
	ShovelerLightSpot *spotlights[6];
} ShovelerLightPoint;

ShovelerLightPoint *shovelerLightPointCreate(ShovelerVector3 position, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color);

#endif
