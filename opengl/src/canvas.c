#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "shoveler/material/tile_sprite.h"
#include "shoveler/camera.h"
#include "shoveler/canvas.h"
#include "shoveler/model.h"
#include "shoveler/light.h"
#include "shoveler/log.h"

ShovelerCanvas *shovelerCanvasCreate()
{
	ShovelerCanvas *canvas = malloc(sizeof(ShovelerCanvas));
	canvas->tileSprites = g_queue_new();
	return canvas;
}

int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, const ShovelerCanvasTileSprite *tileSprite)
{
	g_queue_push_tail(canvas->tileSprites, (gpointer) tileSprite);
	return g_queue_get_length(canvas->tileSprites) - 1;
}

bool shovelerCanvasRender(ShovelerCanvas *canvas, ShovelerMaterial *tileSpriteMaterial, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	// since we are only changing uniform pointer values per layer, we can reuse the same shader for all of them
	ShovelerShader *shader = shovelerSceneGenerateShader(scene, camera, light, model, tileSpriteMaterial, NULL);

	shovelerRenderStateEnableBlend(renderState, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(GList *iter = canvas->tileSprites->head; iter != NULL; iter = iter->next) {
		const ShovelerCanvasTileSprite *tileSprite = iter->data;

		shovelerMaterialTileSpriteSetActive(tileSpriteMaterial, tileSprite);

		if(!shovelerShaderUse(shader)) {
			shovelerLogWarning("Failed to use shader for sprite %p of canvas %p with tile sprite material %p, scene %p, camera %p, light %p and model %p.", tileSprite, canvas, tileSpriteMaterial, scene, camera, light, model);
			return false;
		}

		if(!shovelerModelRender(model)) {
			shovelerLogWarning("Failed to render model %p for for sprite %p of canvas %p with tile sprite material %p in scene %p for camera %p and light %p.", model, tileSprite, canvas, tileSpriteMaterial, scene, camera, light);
			return false;
		}

		shovelerRenderStateEnableDepthTest(renderState, GL_EQUAL);
	}

	return true;
}

void shovelerCanvasFree(ShovelerCanvas *canvas)
{
	if(canvas == NULL) {
		return;
	}

	g_queue_free(canvas->tileSprites);
	free(canvas);
}
