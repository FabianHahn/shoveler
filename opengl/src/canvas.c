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
	canvas->tileSpriteMaterial = shovelerMaterialTileSpriteCreate();
	canvas->tileSprites = g_queue_new();
}

int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, ShovelerCanvasTileSprite tileSprite)
{
	ShovelerCanvasTileSprite *tileSpriteCopy = malloc(sizeof(ShovelerCanvasTileSprite));
	memcpy(tileSpriteCopy, &tileSprite, sizeof(ShovelerCanvasTileSprite));
	g_queue_push_tail(canvas->tileSprites, tileSpriteCopy);
	return g_queue_get_length(canvas->tileSprites) - 1;
}

bool shovelerCanvasRender(ShovelerCanvas *canvas, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerSceneRenderPassOptions *options)
{
	// since we are only changing uniform pointer values per layer, we can reuse the same shader for all of them
	ShovelerShader *shader = shovelerSceneGenerateShader(scene, camera, light, model, canvas->tileSpriteMaterial, NULL);

	// enable alpha blending
	if(options->blend && (options->blendSourceFactor != GL_SRC_ALPHA || options->blendDestinationFactor != GL_ONE_MINUS_SRC_ALPHA)) {
		// always use shader output, and ignore existing output if shader writes output
		options->blendSourceFactor = GL_SRC_ALPHA;
		options->blendDestinationFactor = GL_ONE_MINUS_SRC_ALPHA;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	for(GList *iter = canvas->tileSprites->head; iter != NULL; iter = iter->next) {
		const ShovelerCanvasTileSprite *tileSprite = iter->data;

		shovelerMaterialTileSpriteSetActive(canvas->tileSpriteMaterial, tileSprite);

		if(!shovelerShaderUse(shader)) {
			shovelerLogWarning("Failed to use shader for sprite %p of canvas %p with tile sprite material %p, scene %p, camera %p, light %p and model %p.", tileSprite, canvas, canvas->tileSpriteMaterial, scene, camera, light, model);
			return false;
		}

		if(!shovelerModelRender(model)) {
			shovelerLogWarning("Failed to render model %p for for sprite %p of canvas %p with tile sprite material %p in scene %p for camera %p and light %p.", model, tileSprite, canvas, canvas->tileSpriteMaterial, scene, camera, light);
			return false;
		}

		if (options->depthTest && options->depthFunction != GL_EQUAL) {
			options->depthFunction = GL_EQUAL;
			glDepthFunc(GL_EQUAL);
		}
	}

	return true;
}

void shovelerCanvasFree(ShovelerCanvas *canvas)
{
	g_queue_free_full(canvas->tileSprites, free);
	shovelerMaterialFree(canvas->tileSpriteMaterial);
	free(canvas);
}
