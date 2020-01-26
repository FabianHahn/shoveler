#ifndef SHOVELER_CANVAS_H
#define SHOVELER_CANVAS_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/material.h>
#include <shoveler/scene.h>
#include <shoveler/tileset.h>
#include <shoveler/types.h>

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerFontAtlasTextureStruct ShovelerFontAtlasTexture; // forward declaration: font_atlas_texture.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h

typedef struct ShovelerCanvasStruct {
	int numLayers;
	/** array of size numLayers, wher each element is a list of (ShovelerSprite *) */
	GQueue **layers;
} ShovelerCanvas;

ShovelerCanvas *shovelerCanvasCreate(int numLayers);
/** Adds a sprite to the canvas, with the caller retaining ownership over it and changes to it being reflected live. Returns the id of the added sprite. */
int shovelerCanvasAddSprite(ShovelerCanvas *canvas, int layerId, ShovelerSprite *sprite);
bool shovelerCanvasRender(ShovelerCanvas *canvas, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
void shovelerCanvasFree(ShovelerCanvas *canvas);

#endif
