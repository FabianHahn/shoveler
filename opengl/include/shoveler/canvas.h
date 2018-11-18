#ifndef SHOVELER_CANVAS_H
#define SHOVELER_CANVAS_H

#include <glib.h>

#include <shoveler/tileset.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerTileset *tileset;
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	ShovelerVector2 position;
	ShovelerVector2 size;
} ShovelerCanvasTileSprite;

typedef struct {
	GQueue *tileSprites;
} ShovelerCanvas;

ShovelerCanvas *shovelerCanvasCreate();
int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, ShovelerCanvasTileSprite tileSprite);
void shovelerCanvasFree(ShovelerCanvas *canvas);

#endif
