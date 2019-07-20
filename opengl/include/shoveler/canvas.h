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

typedef struct {
	ShovelerFontAtlasTexture *fontAtlasTexture;
	const char *text;
	ShovelerVector2 position;
	float size;
} ShovelerCanvasTextSprite;

typedef struct {
	ShovelerTileset *tileset;
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	ShovelerVector2 position;
	ShovelerVector2 size;
} ShovelerCanvasTileSprite;

typedef enum {
	SHOVELER_CANVAS_SPRITE_TYPE_TEXT,
	SHOVELER_CANVAS_SPRITE_TYPE_TILE,
} ShovelerCanvasSpriteType;

typedef struct {
	ShovelerCanvasSpriteType type;
	union {
		const ShovelerCanvasTextSprite *text;
		const ShovelerCanvasTileSprite *tile;
	} value;
} ShovelerCanvasSprite;

typedef struct ShovelerCanvasStruct {
	/** list of (ShovelerCanvasSprite *) */
	GQueue *sprites;
} ShovelerCanvas;

ShovelerCanvas *shovelerCanvasCreate();
/** Adds a text sprite to the canvas, with the caller retaining ownership over it and changes to it being reflected live. Returns the id of the added sprite. */
int shovelerCanvasAddTextSprite(ShovelerCanvas *canvas, const ShovelerCanvasTextSprite *textSprite);
/** Adds a tile sprite to the canvas, with the caller retaining ownership over it and changes to it being reflected live. Returns the id of the added sprite. */
int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, const ShovelerCanvasTileSprite *tileSprite);
bool shovelerCanvasRender(ShovelerCanvas *canvas, ShovelerMaterial *textMaterial, ShovelerMaterial *tileSpriteMaterial, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
void shovelerCanvasFree(ShovelerCanvas *canvas);

#endif
