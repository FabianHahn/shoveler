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
	/** list of (ShovelerSprite *) */
	GQueue *sprites;
} ShovelerCanvas;

ShovelerCanvas *shovelerCanvasCreate();
/** Adds a sprite to the canvas, with the caller retaining ownership over it and changes to it being reflected live. Returns the id of the added sprite. */
int shovelerCanvasAddSprite(ShovelerCanvas *canvas, ShovelerSprite *sprite);
bool shovelerCanvasRender(ShovelerCanvas *canvas, ShovelerMaterial *textMaterial, ShovelerMaterial *tileSpriteMaterial, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
void shovelerCanvasFree(ShovelerCanvas *canvas);

#endif
