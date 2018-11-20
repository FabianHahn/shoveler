#include <stdlib.h> // malloc, free

#include "shoveler/material/canvas.h"
#include "shoveler/canvas.h"
#include "shoveler/light.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"

typedef struct {
	ShovelerMaterial *material;
	ShovelerCanvas *canvas;
	bool manageCanvas;
} CanvasData;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerSceneRenderPassOptions options);
static void freeTilemap(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialCanvasCreate(ShovelerCanvas *canvas, bool manageCanvas)
{
	CanvasData *canvasData = malloc(sizeof(CanvasData));
	canvasData->material = shovelerMaterialCreateUnmanaged(0);
	canvasData->material->data = canvasData;
	canvasData->material->render = render;
	canvasData->material->freeData = freeTilemap;
	canvasData->canvas = canvas;
	canvasData->manageCanvas = manageCanvas;

	return canvasData->material;
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerSceneRenderPassOptions options)
{
	CanvasData *canvasData = material->data;

	ShovelerSceneRenderPassOptions currentOptions = options;
	bool rendered = shovelerCanvasRender(canvasData->canvas, scene, camera, light, model, &currentOptions);

	// reset back to original blending and depth test mode
	if(options.blend && (currentOptions.blendSourceFactor != options.blendSourceFactor || currentOptions.blendDestinationFactor != options.blendDestinationFactor)) {
		glBlendFunc(options.blendSourceFactor, options.blendDestinationFactor);
	}
	if(options.depthTest && currentOptions.depthFunction != options.depthFunction) {
		glDepthFunc(options.depthFunction);
	}

	return rendered;
}

static void freeTilemap(ShovelerMaterial *material)
{
	CanvasData *canvasData = material->data;

	if(canvasData->manageCanvas) {
		shovelerCanvasFree(canvasData->canvas);
	}

	free(canvasData);
}
