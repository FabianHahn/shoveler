#ifndef SHOVELER_CANVAS_H
#define SHOVELER_CANVAS_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/material.h>
#include <shoveler/scene.h>
#include <shoveler/tileset.h>
#include <shoveler/types.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerModelStruct; // forward declaration: model.h

typedef struct {
	ShovelerTileset *tileset;
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	ShovelerVector2 position;
	ShovelerVector2 size;
} ShovelerCanvasTileSprite;

typedef struct {
	ShovelerMaterial *tileSpriteMaterial;
	GQueue *tileSprites;
} ShovelerCanvas;

ShovelerCanvas *shovelerCanvasCreate();
/** Adds a tile sprite to the canvas, with the caller retaining ownership over it and changes to it being reflected live. Returns the id of the added sprite. */
int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, const ShovelerCanvasTileSprite *tileSprite);
bool shovelerCanvasRender(ShovelerCanvas *canvas, struct ShovelerMaterialStruct *tileSpriteMaterial, ShovelerScene *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, ShovelerRenderState *renderState);
void shovelerCanvasFree(ShovelerCanvas *canvas);

#endif
