#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "shoveler/canvas.h"

ShovelerCanvas *shovelerCanvasCreate()
{
	ShovelerCanvas *canvas = malloc(sizeof(ShovelerCanvas));
	canvas->tileSprites = g_queue_new();
}

int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, ShovelerCanvasTileSprite tileSprite)
{
	ShovelerCanvasTileSprite *tileSpriteCopy = malloc(sizeof(ShovelerCanvasTileSprite));
	memcpy(tileSpriteCopy, &tileSprite, sizeof(ShovelerCanvasTileSprite));
	g_queue_push_tail(canvas->tileSprites, tileSpriteCopy);
	return g_queue_get_length(canvas->tileSprites) - 1;
}

void shovelerCanvasFree(ShovelerCanvas *canvas)
{
	g_queue_free_full(canvas->tileSprites, free);
	free(canvas);
}
