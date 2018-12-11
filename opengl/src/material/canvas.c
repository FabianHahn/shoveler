#include <stdlib.h> // malloc, free

#include "shoveler/material/canvas.h"
#include "shoveler/material/tile_sprite.h"
#include "shoveler/canvas.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"

typedef struct {
	ShovelerMaterial *material;
	ShovelerMaterial *tileSpriteMaterial;
	ShovelerCanvas *activeCanvas;
} MaterialData;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeTilemap(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialCanvasCreate()
{
	MaterialData *materialData = malloc(sizeof(MaterialData));
	materialData->material = shovelerMaterialCreateUnmanaged(0);
	materialData->material->data = materialData;
	materialData->material->render = render;
	materialData->material->freeData = freeTilemap;
	materialData->tileSpriteMaterial = shovelerMaterialTileSpriteCreate();
	materialData->activeCanvas = NULL;

	return materialData->material;
}

void shovelerMaterialCanvasSetActive(ShovelerMaterial *material, ShovelerCanvas *canvas)
{
	MaterialData *materialData = material->data;
	materialData->activeCanvas = canvas;
}

ShovelerMaterial *shovelerMaterialCanvasGetTileSpriteMaterial(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;
	return materialData->tileSpriteMaterial;
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	MaterialData *materialData = material->data;

	if(materialData->activeCanvas == NULL) {
		shovelerLogWarning("Failed to render canvas material %p without an active canvas.", material);
		return false;
	}

	return shovelerCanvasRender(materialData->activeCanvas, materialData->tileSpriteMaterial, scene, camera, light, model, renderState);
}

static void freeTilemap(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;
	shovelerMaterialFree(materialData->tileSpriteMaterial);
	free(materialData);
}
