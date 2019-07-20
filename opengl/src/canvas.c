#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "shoveler/material/text.h"
#include "shoveler/material/tile_sprite.h"
#include "shoveler/camera.h"
#include "shoveler/canvas.h"
#include "shoveler/model.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/shader.h"

ShovelerCanvas *shovelerCanvasCreate()
{
	ShovelerCanvas *canvas = malloc(sizeof(ShovelerCanvas));
	canvas->sprites = g_queue_new();
	return canvas;
}

int shovelerCanvasAddTextSprite(ShovelerCanvas *canvas, const ShovelerCanvasTextSprite *textSprite)
{
	ShovelerCanvasSprite *sprite = malloc(sizeof(ShovelerCanvasSprite));
	sprite->type = SHOVELER_CANVAS_SPRITE_TYPE_TEXT;
	sprite->value.text = textSprite;

	g_queue_push_tail(canvas->sprites, (gpointer) sprite);
	return g_queue_get_length(canvas->sprites) - 1;
}

int shovelerCanvasAddTileSprite(ShovelerCanvas *canvas, const ShovelerCanvasTileSprite *tileSprite)
{
	ShovelerCanvasSprite *sprite = malloc(sizeof(ShovelerCanvasSprite));
	sprite->type = SHOVELER_CANVAS_SPRITE_TYPE_TILE;
	sprite->value.tile = tileSprite;

	g_queue_push_tail(canvas->sprites, (gpointer) sprite);
	return g_queue_get_length(canvas->sprites) - 1;
}

bool shovelerCanvasRender(ShovelerCanvas *canvas, ShovelerMaterial *textMaterial, ShovelerMaterial *tileSpriteMaterial, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	// since we are only changing uniform pointer values per sprite, we can reuse the same shader for all of them
	ShovelerShader *tileShader = shovelerSceneGenerateShader(scene, camera, light, model, tileSpriteMaterial, NULL);

	shovelerRenderStateEnableBlend(renderState, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(GList *iter = canvas->sprites->head; iter != NULL; iter = iter->next) {
		ShovelerCanvasSprite *sprite = iter->data;

		switch(sprite->type) {
			case SHOVELER_CANVAS_SPRITE_TYPE_TEXT: {
				const ShovelerCanvasTextSprite *textSprite = sprite->value.text;
				shovelerMaterialTextSetActiveFontAtlasTexture(textMaterial, textSprite->fontAtlasTexture);
				shovelerMaterialTextSetActiveText(textMaterial, textSprite->text, textSprite->position, textSprite->size);

				if(!shovelerMaterialRender(textMaterial, scene, camera, light, model, renderState)) {
					shovelerLogWarning("Failed to render material for text sprite %p of canvas %p with text material %p, scene %p, camera %p, light %p and model %p.", textSprite, canvas, textMaterial, scene, camera, light, model);
					return false;
				}
			} break;
			case SHOVELER_CANVAS_SPRITE_TYPE_TILE: {
				const ShovelerCanvasTileSprite *tileSprite = sprite->value.tile;

				shovelerMaterialTileSpriteSetActive(tileSpriteMaterial, tileSprite);

				if(!shovelerShaderUse(tileShader)) {
					shovelerLogWarning("Failed to use shader for tile sprite %p of canvas %p with tile sprite material %p, scene %p, camera %p, light %p and model %p.", tileSprite, canvas, tileSpriteMaterial, scene, camera, light, model);
					return false;
				}

				if(!shovelerModelRender(model)) {
					shovelerLogWarning("Failed to render model %p for for tile sprite %p of canvas %p with tile sprite material %p in scene %p for camera %p and light %p.", model, tileSprite, canvas, tileSpriteMaterial, scene, camera, light);
					return false;
				}

				if(!tileSpriteMaterial->screenspace) {
					shovelerRenderStateEnableDepthTest(renderState, GL_EQUAL);
				}
			} break;
		}
	}

	return true;
}

void shovelerCanvasFree(ShovelerCanvas *canvas)
{
	if(canvas == NULL) {
		return;
	}

	g_queue_free_full(canvas->sprites, free);
	free(canvas);
}
