#ifndef SHOVELER_MATERIAL_CANVAS_H
#define SHOVELER_MATERIAL_CANVAS_H

#include <stdbool.h> // bool

#include <shoveler/canvas.h>
#include <shoveler/material.h>

ShovelerMaterial *shovelerMaterialCanvasCreate(ShovelerCanvas *canvas, bool manageCanvas);

#endif
