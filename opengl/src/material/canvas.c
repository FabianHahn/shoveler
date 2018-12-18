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
	ShovelerVector2 activeRegionPosition;
	ShovelerVector2 activeRegionSize;
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
	materialData->activeRegionPosition = shovelerVector2(0.0f, 0.0f);
	materialData->activeRegionSize = shovelerVector2(1.0f, 1.0f);

	shovelerUniformMapInsert(materialData->tileSpriteMaterial->uniforms, "regionPosition", shovelerUniformCreateVector2Pointer(&materialData->activeRegionPosition));
	shovelerUniformMapInsert(materialData->tileSpriteMaterial->uniforms, "regionSize", shovelerUniformCreateVector2Pointer(&materialData->activeRegionSize));

	return materialData->material;
}

void shovelerMaterialCanvasSetActive(ShovelerMaterial *material, ShovelerCanvas *canvas)
{
	MaterialData *materialData = material->data;
	materialData->activeCanvas = canvas;
}

void shovelerMaterialCanvasSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 position, ShovelerVector2 size)
{
	MaterialData *materialData = material->data;
	materialData->activeRegionPosition = position;
	materialData->activeRegionSize = size;
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
