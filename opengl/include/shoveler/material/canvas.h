#ifndef SHOVELER_MATERIAL_CANVAS_H
#define SHOVELER_MATERIAL_CANVAS_H

#include <stdbool.h> // bool

#include <shoveler/canvas.h>
#include <shoveler/material.h>

ShovelerMaterial *shovelerMaterialCanvasCreate();
void shovelerMaterialCanvasSetActive(ShovelerMaterial *material, ShovelerCanvas *canvas);
ShovelerMaterial *shovelerMaterialCanvasGetTileSpriteMaterial(ShovelerMaterial *material);

#endif
