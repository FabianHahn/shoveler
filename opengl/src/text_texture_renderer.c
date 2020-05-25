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

	renderer->textCanvas = shovelerCanvasCreate(/* numLayers */ 1);
	renderer->textSprite = shovelerSpriteTextCreate(renderer->textMaterial, fontAtlasTexture, fontAtlasTexture->fontAtlas->fontSize, /* color */ shovelerVector4(1.0f, 1.0f, 1.0f, 1.0f));
	shovelerCanvasAddSprite(renderer->textCanvas, /* layerId */ 0, renderer->textSprite);
	shovelerMaterialCanvasSetActive(renderer->canvasMaterial, renderer->textCanvas);

	return renderer;
}

ShovelerTexture *shovelerTextTextureRendererRender(ShovelerTextTextureRenderer *renderer, const char *text, ShovelerRenderState *renderState)
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

void shovelerTextTextureRendererFree(ShovelerTextTextureRenderer *renderer)
{
	shovelerSpriteFree(renderer->textSprite);
	shovelerCanvasFree(renderer->textCanvas);
	shovelerSceneRemoveModel(renderer->textScene, renderer->textModel);
	shovelerDrawableFree(renderer->textQuad);
	shovelerMaterialFree(renderer->textMaterial);
	shovelerMaterialFree(renderer->canvasMaterial);
	shovelerSceneFree(renderer->textScene);
	free(renderer);
}
