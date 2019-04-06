#ifndef SHOVELER_MATERIAL_PARTICLE_H
#define SHOVELER_MATERIAL_PARTICLE_H

#include <shoveler/material.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialParticleCreate(struct ShovelerShaderCacheStruct *shaderCache, ShovelerVector3 color);

#endif
