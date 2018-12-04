#include <stdlib.h> // malloc, free

#include "shoveler/material/chunk.h"
#include "shoveler/chunk.h"
#include "shoveler/light.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"

typedef struct {
	ShovelerMaterial *material;
	ShovelerChunk *chunk;
	bool manageChunk;
} MaterialData;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialChunkCreate(ShovelerChunk *chunk, bool manageChunk)
{
	MaterialData *materialData = malloc(sizeof(MaterialData));
	materialData->material = shovelerMaterialCreateUnmanaged(0);
	materialData->material->data = materialData;
	materialData->material->render = render;
	materialData->material->freeData = freeMaterialData;
	materialData->chunk = chunk;
	materialData->manageChunk = manageChunk;

	return materialData->material;
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	MaterialData *materialData = material->data;
	return shovelerChunkRender(materialData->chunk, scene, camera, light, model, renderState);
}

static void freeMaterialData(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;

	if(materialData->manageChunk) {
		shovelerChunkFree(materialData->chunk);
	}

	free(materialData);
}
