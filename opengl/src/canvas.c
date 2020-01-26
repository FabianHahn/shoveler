#include <assert.h> // assert
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "shoveler/camera.h"
#include "shoveler/canvas.h"
#include "shoveler/model.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/shader.h"
#include "shoveler/sprite.h"

ShovelerCanvas *shovelerCanvasCreate(int numLayers)
{
	assert(numLayers > 0);

	ShovelerCanvas *canvas = malloc(sizeof(ShovelerCanvas));
	canvas->numLayers = numLayers;
	canvas->layers = malloc((size_t) numLayers * sizeof(GQueue *));

	for(int layerId = 0; layerId < canvas->numLayers; layerId++) {
		canvas->layers[layerId] = g_queue_new();
	}

	return canvas;
}

int shovelerCanvasAddSprite(ShovelerCanvas *canvas, int layerId, ShovelerSprite *sprite)
{
	assert(layerId >= 0);
	assert(layerId < canvas->numLayers);

	GQueue *layer = canvas->layers[layerId];

	g_queue_push_tail(layer, (gpointer) sprite);
	return g_queue_get_length(layer) - 1;
}

bool shovelerCanvasRender(ShovelerCanvas *canvas, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	shovelerRenderStateEnableBlend(renderState, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(int layerId = 0; layerId < canvas->numLayers; layerId++) {
		GQueue *layer = canvas->layers[layerId];

		for(GList *iter = layer->head; iter != NULL; iter = iter->next) {
			ShovelerSprite *sprite = iter->data;

			if(!shovelerSpriteRender(sprite, regionPosition, regionSize, scene, camera, light, model, renderState)) {
				shovelerLogWarning("Failed to render sprite %p of canvas %p to scene %p, camera %p, light %p and model %p.", sprite, canvas, scene, camera, light, model);
				return false;
			}

			if(!sprite->material->screenspace) {
				shovelerRenderStateEnableDepthTest(renderState, GL_EQUAL);
			}
		}
	}

	return true;
}

void shovelerCanvasFree(ShovelerCanvas *canvas)
{
	if(canvas == NULL) {
		return;
	}

	for(int layerId = 0; layerId < canvas->numLayers; layerId++) {
		g_queue_free(canvas->layers[layerId]);
	}

	free(canvas->layers);
	free(canvas);
}
