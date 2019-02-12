#ifndef SHOVELER_CANVAS_H
#define SHOVELER_CANVAS_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/collider.h>
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
	/* private */ ShovelerColliders *colliders;
	/* private */ GQueue *tileSprites;
} ShovelerCanvas;

/** Creates a canvas using a set of colliders, over which the caller retains ownership. */
ShovelerCanvas *shovelerCanvasCreate(ShovelerColliders *colliders);
/** Adds a tile sprite to the canvas, with the caller retaining ownership over it and changes to it being reflected live. Returns the id of the added sprite. */
int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, ShovelerCanvasTileSprite *tileSprite);
bool shovelerCanvasMoveTileSprite(ShovelerCanvas *canvas, ShovelerCanvasTileSprite *tileSprite, ShovelerVector2 targetPosition);
bool shovelerCanvasRender(ShovelerCanvas *canvas, struct ShovelerMaterialStruct *tileSpriteMaterial, ShovelerScene *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, ShovelerRenderState *renderState);
void shovelerCanvasFree(ShovelerCanvas *canvas);

#endif
