#ifndef SHOVELER_MATERIAL_COLOR_H
#define SHOVELER_MATERIAL_COLOR_H

#include <shoveler/material.h>
#include <shoveler/types.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialColorCreate(struct ShovelerShaderCacheStruct *shaderCache, ShovelerVector3 color);

#endif
