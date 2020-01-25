#include <math.h> // ceilf
#include <stdlib.h> // malloc free
#include <string.h> // strdup

#include "shoveler/drawable/quad.h"
#include "shoveler/material/canvas.h"
#include "shoveler/material/text.h"
#include "shoveler/font_atlas.h"
#include "shoveler/font_atlas_texture.h"
#include "shoveler/framebuffer.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"
#include "shoveler/shader_cache.h"
#include "shoveler/sprite/text.h"
#include "shoveler/text_texture_renderer.h"
#include "shoveler/texture.h"

static void freeTextTexture(void *textTexturePointer);
static ShovelerTexture *render(ShovelerTextTextureRenderer *renderer, const char *text, ShovelerRenderState *renderState);

ShovelerTextTextureRenderer *shovelerTextTextureRendererCreate(ShovelerFontAtlasTexture *fontAtlasTexture, ShovelerShaderCache *shaderCache)
{
	ShovelerTextTextureRenderer *renderer = malloc(sizeof(ShovelerTextTextureRenderer));
	renderer->fontAtlasTexture = fontAtlasTexture;
	renderer->textScene = shovelerSceneCreate(shaderCache);
	renderer->canvasMaterial = shovelerMaterialCanvasCreate(shaderCache, /* screenspace */ true);
	renderer->textMaterial = shovelerMaterialTextCreate(shaderCache, /* screenspace */ true);
	renderer->textQuad = shovelerDrawableQuadCreate();
	renderer->textModel = shovelerModelCreate(renderer->textQuad, renderer->canvasMaterial);
	shovelerSceneAddModel(renderer->textScene, renderer->textModel);

	renderer->textCanvas = shovelerCanvasCreate();
	renderer->textSprite = shovelerSpriteTextCreate(renderer->textMaterial, fontAtlasTexture, fontAtlasTexture->fontAtlas->fontSize, /* color */ shovelerVector4(1.0f, 1.0f, 1.0f, 1.0f));
	shovelerCanvasAddSprite(renderer->textCanvas, renderer->textSprite);
	shovelerMaterialCanvasSetActive(renderer->canvasMaterial, renderer->textCanvas);

	renderer->textures = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeTextTexture);

	return renderer;
}

ShovelerTexture *shovelerTextTextureRendererRender(ShovelerTextTextureRenderer *renderer, const char *text, ShovelerRenderState *renderState)
{
	ShovelerTextTexture *textTexture = g_hash_table_lookup(renderer->textures, text);
	if(textTexture == NULL) {
		ShovelerTexture *texture = render(renderer, text, renderState);

		if(texture != NULL) {
			textTexture = malloc(sizeof(ShovelerTextTextureRenderer));
			textTexture->text = strdup(text);
			textTexture->texture = texture;
			g_hash_table_insert(renderer->textures, textTexture->text, textTexture);
		}
	}

	return textTexture != NULL ? textTexture->texture : NULL;
}

void shovelerTextTextureRendererFree(ShovelerTextTextureRenderer *renderer)
{
	g_hash_table_destroy(renderer->textures);

	shovelerSpriteFree(renderer->textSprite);
	shovelerCanvasFree(renderer->textCanvas);
	shovelerSceneRemoveModel(renderer->textScene, renderer->textModel);
	shovelerDrawableFree(renderer->textQuad);
	shovelerMaterialFree(renderer->textMaterial);
	shovelerMaterialFree(renderer->canvasMaterial);
	shovelerSceneFree(renderer->textScene);
	free(renderer);
}

static void freeTextTexture(void *textTexturePointer)
{
	ShovelerTextTexture *textTexture = textTexturePointer;
	free(textTexture->text);
	shovelerTextureFree(textTexture->texture);
	free(textTexture);
}

static ShovelerTexture *render(ShovelerTextTextureRenderer *renderer, const char *text, ShovelerRenderState *renderState)
{
	float currentWidth = 0.0f;
	float currentHeightTop = 0.0f;
	float currentHeightBottom = 0.0f;
	float currentOriginX = 0.0f;

	for(const char *c = text; *c != '\0'; c++) {
		unsigned char character = *((unsigned char *) c);

		currentWidth = currentOriginX;

		ShovelerFontAtlasGlyph *glyph = shovelerFontAtlasGetGlyph(renderer->fontAtlasTexture->fontAtlas, character);

		currentWidth += glyph->bearingX;
		currentWidth += glyph->width;

		if(glyph->bearingY > currentHeightTop) {
			currentHeightTop = glyph->bearingY;
		}

		float bottomExtent = glyph->height - glyph->bearingY;
		if(bottomExtent > currentHeightBottom) {
			currentHeightBottom = bottomExtent;
		}

		currentOriginX += glyph->advance / 64.0f;
	}

	unsigned int width = (unsigned int) ceilf(currentWidth);
	unsigned int height = (unsigned int) ceilf(currentHeightBottom + currentHeightTop);

	shovelerFontAtlasTextureUpdate(renderer->fontAtlasTexture);
	shovelerSpriteTextSetContent(renderer->textSprite, text, false);
	renderer->textSprite->position = shovelerVector2(0.0f, currentHeightBottom);

	shovelerMaterialCanvasSetActiveRegion(renderer->canvasMaterial, shovelerVector2(0.5f * width, 0.5f * height), shovelerVector2(width, height));

	ShovelerFramebuffer *framebuffer = shovelerFramebufferCreateColorOnly(width, height, 1, 1, 8);
	shovelerSceneRenderFrame(renderer->textScene, NULL, framebuffer, renderState);

	ShovelerTexture *texture = framebuffer->renderTarget;
	shovelerFramebufferFree(framebuffer, /* keepTargets */ true);
	return texture;
}
