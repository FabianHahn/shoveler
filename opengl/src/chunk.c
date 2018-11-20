#include <stdlib.h> // malloc, free

#include "shoveler/material/tilemap.h"
#include "shoveler/camera.h"
#include "shoveler/canvas.h"
#include "shoveler/chunk.h"
#include "shoveler/model.h"
#include "shoveler/light.h"
#include "shoveler/log.h"

ShovelerChunk *shovelerChunkCreate()
{
	ShovelerChunk *chunk = malloc(sizeof(ShovelerChunk));
	chunk->tilemapMaterial = shovelerMaterialTilemapCreate();
	chunk->layers = g_queue_new();

	return chunk;
}

int shovelerChunkAddCanvasLayer(ShovelerChunk *chunk, ShovelerCanvas *canvas)
{
	ShovelerChunkLayer *layer = malloc(sizeof(ShovelerChunkLayer));
	layer->type = SHOVELER_CHUNK_LAYER_TYPE_CANVAS;
	layer->value.canvas = canvas;

	g_queue_push_tail(chunk->layers, layer);
	return g_queue_get_length(chunk->layers) - 1;
}

int shovelerChunkAddTilemapLayer(ShovelerChunk *chunk, ShovelerTilemap *tilemap)
{
	ShovelerChunkLayer *layer = malloc(sizeof(ShovelerChunkLayer));
	layer->type = SHOVELER_CHUNK_LAYER_TYPE_TILEMAP;
	layer->value.tilemap = tilemap;

	g_queue_push_tail(chunk->layers, layer);
	return g_queue_get_length(chunk->layers) - 1;
}

bool shovelerChunkRender(ShovelerChunk *chunk, ShovelerScene *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, ShovelerSceneRenderPassOptions *options)
{
	for(GList *iter = chunk->layers->head; iter != NULL; iter = iter->next) {
		ShovelerChunkLayer *layer = iter->data;

		switch(layer->type) {
			case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
				if(!shovelerCanvasRender(layer->value.canvas, scene, camera, light, model, options)) {
					shovelerLogWarning("Failed to render canvas %p layer of chunk %p.", layer->value.canvas, chunk);
					return false;
				}
				break;
			case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
				if(!shovelerTilemapRender(layer->value.tilemap, chunk->tilemapMaterial, scene, camera, light, model, options)) {
					shovelerLogWarning("Failed to render tilemap %p layer of chunk %p.", layer->value.tilemap, chunk);
					return false;
				}
				break;
			default:
				shovelerLogWarning("Unknown canvas layer type %d.", layer->type);
				return false;
		}
	}

	return true;
}

void shovelerChunkFree(ShovelerChunk *chunk)
{
	g_queue_free_full(chunk->layers, free);
	shovelerMaterialFree(chunk->tilemapMaterial);
	free(chunk);
}
