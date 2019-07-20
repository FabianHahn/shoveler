#ifndef SHOVELER_MATERIAL_CANVAS_H
#define SHOVELER_MATERIAL_CANVAS_H

#include <stdbool.h> // bool

#include <shoveler/canvas.h>
#include <shoveler/material.h>
#include <shoveler/types.h>

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialCanvasCreate(ShovelerShaderCache *shaderCache, bool screenspace);
void shovelerMaterialCanvasSetActive(ShovelerMaterial *material, ShovelerCanvas *canvas);
void shovelerMaterialCanvasSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 position, ShovelerVector2 size);
ShovelerMaterial *shovelerMaterialCanvasGetTextMaterial(ShovelerMaterial *material);
ShovelerMaterial *shovelerMaterialCanvasGetTileSpriteMaterial(ShovelerMaterial *material);

#endif
