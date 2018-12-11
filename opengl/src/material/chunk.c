#include <stdlib.h> // malloc, free

#include "shoveler/material/chunk.h"
#include "shoveler/chunk.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"

typedef struct {
	ShovelerMaterial *material;
	ShovelerChunk *activeChunk;
} MaterialData;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialChunkCreate()
{
	MaterialData *materialData = malloc(sizeof(MaterialData));
	materialData->material = shovelerMaterialCreateUnmanaged(0);
	materialData->material->data = materialData;
	materialData->material->render = render;
	materialData->material->freeData = freeMaterialData;
	materialData->activeChunk = NULL;

	return materialData->material;
}

void shovelerMaterialChunkSetActive(ShovelerMaterial *material, ShovelerChunk *chunk)
{
	MaterialData *materialData = material->data;
	materialData->activeChunk = chunk;
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	MaterialData *materialData = material->data;

	if(materialData->activeChunk == NULL) {
		shovelerLogWarning("Failed to render chunk material %p without an active chunk.", material);
		return false;
	}

	return shovelerChunkRender(materialData->activeChunk, scene, camera, light, model, renderState);
}

static void freeMaterialData(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;
	free(materialData);
}
