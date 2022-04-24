#ifndef SHOVELER_MATERIAL_PARTICLE_H
#define SHOVELER_MATERIAL_PARTICLE_H

#include <shoveler/material.h>
#include <shoveler/types.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial* shovelerMaterialParticleCreate(
    ShovelerShaderCache* shaderCache, ShovelerVector4 color);

#endif
