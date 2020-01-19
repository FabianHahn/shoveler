#ifndef SHOVELER_TEXT_TEXTURE_RENDERER_H
#define SHOVELER_TEXT_TEXTURE_RENDERER_H

#include <stdbool.h> // bool

#include <glad/glad.h>
#include <glib.h>

#include <shoveler/canvas.h>

typedef struct ShovelerDrawableStruct ShovelerDrawable; // forward declaration: drawable.h
typedef struct ShovelerFontAtlasTextureStruct ShovelerFontAtlasTexture; // forward declaration: font_atlas_texture.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerRenderStateStruct ShovelerRenderState; // forward declaration: render_state.h
typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: scene.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h
typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h

typedef struct ShovelerTextTextureStruct {
	char *text;
	ShovelerTexture *texture;
} ShovelerTextTexture;

typedef struct ShovelerTextTextureRendererStruct {
	ShovelerFontAtlasTexture *fontAtlasTexture;
	ShovelerScene *textScene;
	ShovelerMaterial *canvasMaterial;
	ShovelerDrawable *textQuad;
	ShovelerModel *textModel;
	ShovelerCanvas *textCanvas;
	ShovelerSprite *textSprite;
	/* map from texture text to (ShovelerTextTexture *) */
	GHashTable *textures;
} ShovelerTextTextureRenderer;

/** Create a renderer with the caller retaining ownership over the passed font atlas texture. */
ShovelerTextTextureRenderer *shovelerTextTextureRendererCreate(ShovelerFontAtlasTexture *fontAtlasTexture, ShovelerShaderCache *shaderCache);
ShovelerTexture *shovelerTextTextureRendererRender(ShovelerTextTextureRenderer *renderer, const char *text, ShovelerRenderState *renderState);
void shovelerTextTextureRendererFree(ShovelerTextTextureRenderer *renderer);

#endif
