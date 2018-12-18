#ifndef SHOVELER_MATERIAL_CANVAS_H
#define SHOVELER_MATERIAL_CANVAS_H

#include <stdbool.h> // bool

#include <shoveler/canvas.h>
#include <shoveler/material.h>
#include <shoveler/types.h>

ShovelerMaterial *shovelerMaterialCanvasCreate();
void shovelerMaterialCanvasSetActive(ShovelerMaterial *material, ShovelerCanvas *canvas);
void shovelerMaterialCanvasSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 position, ShovelerVector2 size);
ShovelerMaterial *shovelerMaterialCanvasGetTileSpriteMaterial(ShovelerMaterial *material);

#endif
