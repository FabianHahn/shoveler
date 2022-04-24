#ifndef SHOVELER_LIGHT_POINT_H
#define SHOVELER_LIGHT_POINT_H

#include <shoveler/light/spot.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerLight* shovelerLightPointCreate(
    struct ShovelerShaderCacheStruct* shaderCache,
    ShovelerVector3 position,
    int width,
    int height,
    GLsizei samples,
    float ambientFactor,
    float exponentialFactor,
    ShovelerVector3 color);
ShovelerLightSpotShared* shovelerLightPointGetShared(ShovelerLight* light);

#endif
