#ifndef SHOVELER_MATERIAL_DEPTH_H
#define SHOVELER_MATERIAL_DEPTH_H

#include <shoveler/material.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial* shovelerMaterialDepthCreate(ShovelerShaderCache* shaderCache, bool screenspace);

#endif
