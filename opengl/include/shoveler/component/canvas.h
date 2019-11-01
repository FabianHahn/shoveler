#ifndef SHOVELER_COMPONENT_CANVAS_H
#define SHOVELER_COMPONENT_CANVAS_H

typedef struct ShovelerCanvasStruct ShovelerCanvas; // forward declaration: canvas.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewCanvasComponentTypeName = "canvas";
static const char *shovelerViewCanvasTileSpritesOptionKey = "tile_sprites";

ShovelerComponentType *shovelerComponentCreateCanvasType();
ShovelerCanvas *shovelerComponentGetCanvas(ShovelerComponent *component);

#endif
