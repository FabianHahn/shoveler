#ifndef SHOVELER_MATERIAL_COLOR_H
#define SHOVELER_MATERIAL_COLOR_H

#include <shoveler/material.h>
#include <shoveler/types.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialColorCreate(ShovelerShaderCache *shaderCache, bool screenspace, ShovelerVector3 color);

#endif
