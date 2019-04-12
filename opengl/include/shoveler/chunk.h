#ifndef SHOVELER_CHUNK_H
#define SHOVELER_CHUNK_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/canvas.h>
#include <shoveler/collider.h>
#include <shoveler/material.h>
#include <shoveler/scene.h>
#include <shoveler/tilemap.h>
#include <shoveler/types.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerCollider2Struct; // forward declaration: collider.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerModelStruct; // forward declaration: model.h

typedef enum {
	SHOVELER_CHUNK_LAYER_TYPE_CANVAS,
	SHOVELER_CHUNK_LAYER_TYPE_TILEMAP,
} ShovelerChunkLayerType;

typedef union {
	ShovelerCanvas *canvas;
	ShovelerTilemap *tilemap;
} ShovelerChunkLayerValue;

typedef struct {
	ShovelerChunkLayerType type;
	ShovelerChunkLayerValue value;
} ShovelerChunkLayer;

typedef struct ShovelerChunkStruct {
	ShovelerVector2 position;
	ShovelerVector2 size;
	ShovelerBoundingBox2 boundingBox;
	/** list of (ShovelerChunkLayer *) */
	GQueue *layers;
} ShovelerChunk;

ShovelerChunk *shovelerChunkCreate(ShovelerVector2 position, ShovelerVector2 size);
/** Adds a canvas layer to the chunk, not taking ownership of it. */
int shovelerChunkAddCanvasLayer(ShovelerChunk *chunk, ShovelerCanvas *canvas);
/** Adds a layer of tile sprites to the chunk, not taking ownership over the passed tilemap. */
int shovelerChunkAddTilemapLayer(ShovelerChunk *chunk, ShovelerTilemap *tilemap);
bool shovelerChunkIntersect(ShovelerChunk *chunk, const ShovelerBoundingBox2 *object);
bool shovelerChunkRender(ShovelerChunk *chunk, struct ShovelerMaterialStruct *canvasMaterial, struct ShovelerMaterialStruct *tilemapMaterial, ShovelerScene *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, ShovelerRenderState *renderState);
void shovelerChunkFree(ShovelerChunk *chunk);

#endif
