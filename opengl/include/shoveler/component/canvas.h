#ifndef SHOVELER_COMPONENT_CANVAS_H
#define SHOVELER_COMPONENT_CANVAS_H

typedef struct ShovelerCanvasStruct ShovelerCanvas; // forward declaration: canvas.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdCanvas;
static const char *shovelerComponentCanvasOptionKeyTileSprites = "tile_sprites";

ShovelerComponentType *shovelerComponentCreateCanvasType();
ShovelerCanvas *shovelerComponentGetCanvas(ShovelerComponent *component);

#endif
