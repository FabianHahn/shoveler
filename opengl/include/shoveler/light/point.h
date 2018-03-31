#ifndef SHOVELER_LIGHT_POINT_H
#define SHOVELER_LIGHT_POINT_H

#include <shoveler/light/spot.h>

ShovelerLight *shovelerLightPointCreate(ShovelerVector3 position, int width, int height, GLsizei samples, float ambientFactor, float exponentialFactor, ShovelerVector3 color);
ShovelerLightSpotShared *shovelerLightPointGetShared(ShovelerLight *light);

#endif
