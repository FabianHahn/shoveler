#ifndef SHOVELER_MATERIAL_DEPTH_H
#define SHOVELER_MATERIAL_DEPTH_H

#include <shoveler/material.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialDepthCreate(struct ShovelerShaderCacheStruct *shaderCache);

#endif
