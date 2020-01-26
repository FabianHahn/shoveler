#include <stdlib.h> // malloc, free

#include "shoveler/material/canvas.h"
#include "shoveler/material/tilemap.h"
#include "shoveler/camera.h"
#include "shoveler/canvas.h"
#include "shoveler/chunk.h"
#include "shoveler/collider.h"
#include "shoveler/model.h"
#include "shoveler/light.h"
#include "shoveler/log.h"

static ShovelerCollider2 *intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object);

ShovelerChunk *shovelerChunkCreate(ShovelerVector2 position, ShovelerVector2 size)
{
	ShovelerChunk *chunk = malloc(sizeof(ShovelerChunk));
	chunk->position = position;
	chunk->size = size;
	chunk->collider = malloc(sizeof(ShovelerCollider2));
	chunk->collider->boundingBox = shovelerBoundingBox2(
		shovelerVector2LinearCombination(1.0f, chunk->position, -0.5f, chunk->size),
		shovelerVector2LinearCombination(1.0f, chunk->position, 0.5f, chunk->size));
	chunk->collider->intersect = intersectCollider;
	chunk->collider->data = chunk;
	chunk->layers = g_queue_new();

	return chunk;
}

void shovelerChunkUpdatePosition(ShovelerChunk *chunk, ShovelerVector2 position)
{
	chunk->position = position;
	chunk->collider->boundingBox = shovelerBoundingBox2(
		shovelerVector2LinearCombination(1.0f, chunk->position, -0.5f, chunk->size),
		shovelerVector2LinearCombination(1.0f, chunk->position, 0.5f, chunk->size));
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

bool shovelerChunkIntersect(ShovelerChunk *chunk, const ShovelerBoundingBox2 *object)
{
	for(GList *iter = chunk->layers->head; iter != NULL; iter = iter->next) {
		ShovelerChunkLayer *layer = iter->data;

		switch(layer->type) {
			case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
				// no collision representation
				continue;
			case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
				if(shovelerTilemapIntersect(layer->value.tilemap, &chunk->collider->boundingBox, object)) {
					return true;
				}
				break;
			default:
				shovelerLogWarning("Unknown canvas layer type %d.", layer->type);
				return false;
		}
	}

	return false;
}

bool shovelerChunkRender(ShovelerChunk *chunk, ShovelerMaterial *canvasMaterial, ShovelerMaterial *tilemapMaterial, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	for(GList *iter = chunk->layers->head; iter != NULL; iter = iter->next) {
		ShovelerChunkLayer *layer = iter->data;

		switch(layer->type) {
			case SHOVELER_CHUNK_LAYER_TYPE_CANVAS:
				if(!shovelerCanvasRender(layer->value.canvas, chunk->position, chunk->size, scene, camera, light, model, renderState)) {
					shovelerLogWarning("Failed to render canvas %p layer of chunk %p.", layer->value.canvas, chunk);
					return false;
				}
				break;
			case SHOVELER_CHUNK_LAYER_TYPE_TILEMAP:
				if(!shovelerTilemapRender(layer->value.tilemap, /* regionPosition */ shovelerVector2(0.0f, 0.0f), /* regionSize */ shovelerVector2(1.0f, 1.0f), tilemapMaterial, scene, camera, light, model, renderState)) {
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
	if(chunk == NULL) {
		return;
	}

	g_queue_free_full(chunk->layers, free);
	free(chunk->collider);
	free(chunk);
}

static ShovelerCollider2 *intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object)
{
	ShovelerChunk *chunk = collider->data;

	if(shovelerChunkIntersect(chunk, object)) {
		return collider;
	}

	return NULL;
}
