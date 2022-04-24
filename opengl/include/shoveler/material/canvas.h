#ifndef SHOVELER_MATERIAL_CANVAS_H
#define SHOVELER_MATERIAL_CANVAS_H

#include <shoveler/types.h>
#include <stdbool.h> // bool

typedef struct ShovelerCanvasStruct ShovelerCanvas; // forward declaration: material.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial* shovelerMaterialCanvasCreate(ShovelerShaderCache* shaderCache, bool screenspace);
void shovelerMaterialCanvasSetActive(ShovelerMaterial* material, ShovelerCanvas* canvas);
void shovelerMaterialCanvasSetActiveRegion(
    ShovelerMaterial* material, ShovelerVector2 position, ShovelerVector2 size);

#endif
