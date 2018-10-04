#include <assert.h> // assert
#include <limits.h> // UCHAR_MAX
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy
#include <shoveler/material/tilemap.h>
#include <shoveler/scene.h>

#include "shoveler/material/tilemap.h"
#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"
#include "shoveler/shader.h"
#include "shoveler/shader_program.h"

static const char *vertexShaderSource =
		"#version 400\n"
		""
		"uniform mat4 model;\n"
		"uniform mat4 modelNormal;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"uniform mat4 lightView;\n"
		"uniform mat4 lightProjection;\n"
		""
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec2 uv;\n"
		""
		"out vec3 worldPosition;"
		"out vec3 worldNormal;"
		"out vec2 worldUv;"
		"out vec4 lightFrustumPosition4;"
		""
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(position, 1.0);\n"
		"	vec4 worldNormal4 = modelNormal * vec4(normal, 1.0);\n"
		"	worldPosition = worldPosition4.xyz / worldPosition4.w;\n"
		"	worldNormal = worldNormal4.xyz / worldNormal4.w;\n"
		"	worldUv = uv;\n"
		""
		"	lightFrustumPosition4 = lightProjection * lightView * worldPosition4;\n"
		""
		"	gl_Position = projection * view * worldPosition4;\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		"\n"
		"uniform int tilemapWidth;\n"
		"uniform int tilemapHeight;\n"
		"uniform int tilesetWidth;\n"
		"uniform int tilesetHeight;\n"
		"uniform int tilesetId;\n"
		"uniform sampler2D layer;\n"
		"uniform sampler2D tileset;\n"
		"\n"
		"flat in vec2 fragmentPosition;\n"
		"in vec2 fragmentUv;\n"
		""
		"in vec3 worldPosition;\n"
		"in vec3 worldNormal;\n"
		"in vec2 worldUv;\n"
		"in vec4 lightFrustumPosition4;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec3 tile = round(255 * texture2D(layer, worldUv).xyz);\n"
		"	int tileTilesetId = int(tile.z);\n"
		""
		"	if (tileTilesetId == tilesetId) {\n"
		"		vec2 tilemapScaledUv = worldUv * vec2(tilemapWidth, tilemapHeight);"
		"		vec2 tileOffset = tilemapScaledUv - floor(tilemapScaledUv);"
		"		vec3 color = texture2D(tileset, (tile.xy + tileOffset) / vec2(tilesetWidth, tilesetHeight)).rgb;\n"
		"		fragmentColor = vec4(color, 1.0);\n"
		"	} else {\n"
		"		fragmentColor = vec4(0.0);\n"
		"	}\n"
		"}\n";

typedef struct {
	ShovelerMaterial *material;
	int width;
	int height;
	ShovelerSampler *tilemapSampler;
	ShovelerTexture *activeLayerTexture;
	int activeTilesetId;
	ShovelerMaterialTilemapTileset activeTileset;
	/** list of (ShovelerTexture *) */
	GQueue *layers;
	/** list of (ShovelerMaterialTilemapTileset *) */
	GQueue *tilesets;
} Tilemap;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerSceneRenderPassOptions options);
static void freeTilemap(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialTilemapCreate(unsigned char width, unsigned char height)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	Tilemap *tilemap = malloc(sizeof(Tilemap));
	tilemap->material = shovelerMaterialCreate(program);
	tilemap->material->data = tilemap;
	tilemap->material->render = render;
	tilemap->material->freeData = freeTilemap;
	tilemap->width = width;
	tilemap->height = height;
	tilemap->tilemapSampler = shovelerSamplerCreate(false, true);
	tilemap->activeLayerTexture = NULL;
	tilemap->activeTilesetId = 0;
	tilemap->activeTileset = (ShovelerMaterialTilemapTileset){0, 0, NULL, NULL};
	tilemap->layers = g_queue_new();
	tilemap->tilesets = g_queue_new();

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapWidth", shovelerUniformCreateInt(tilemap->width));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapHeight", shovelerUniformCreateInt(tilemap->height));

	shovelerUniformMapInsert(tilemap->material->uniforms, "layer", shovelerUniformCreateTexturePointer(&tilemap->activeLayerTexture, &tilemap->tilemapSampler));

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetId", shovelerUniformCreateIntPointer(&tilemap->activeTilesetId));

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetHeight", shovelerUniformCreateIntPointer(&tilemap->activeTileset.height));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetWidth", shovelerUniformCreateIntPointer(&tilemap->activeTileset.width));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&tilemap->activeTileset.texture, &tilemap->activeTileset.sampler));

	return tilemap->material;
}

int shovelerMaterialTilemapAddLayer(ShovelerMaterial *material, ShovelerTexture *layerTexture)
{
	Tilemap *tilemap = material->data;

	assert(layerTexture->image->width == tilemap->width);
	assert(layerTexture->image->height == tilemap->height);

	g_queue_push_tail(tilemap->layers, layerTexture);
	return g_queue_get_length(tilemap->layers) - 1;
}

int shovelerMaterialTilemapAddTileset(ShovelerMaterial *material, ShovelerMaterialTilemapTileset tileset)
{
	Tilemap *tilemap = material->data;

	assert(tileset.width <= UCHAR_MAX);
	assert(tileset.height <= UCHAR_MAX);

	ShovelerMaterialTilemapTileset *tilesetCopy = malloc(sizeof(ShovelerMaterialTilemapTileset));
	memcpy(tilesetCopy, &tileset, sizeof(ShovelerMaterialTilemapTileset));

	g_queue_push_tail(tilemap->tilesets, tilesetCopy);
	return g_queue_get_length(tilemap->tilesets); // start with one since zero is blank
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerSceneRenderPassOptions options)
{
	Tilemap *tilemap = material->data;

	// since we are only changing uniform pointer values per layer, we can reuse the same shader for all of them
	ShovelerShader *shader = shovelerSceneGenerateShader(scene, camera, light, model, material, NULL);

	// enable alpha blending
	if(options.blend) {
		// always use shader output, and ignore existing output if shader writes output
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	for(GList *iter = tilemap->layers->head; iter != NULL; iter = iter->next) {
		ShovelerTexture *layerTexture = (ShovelerTexture *) iter->data;

		if(iter == tilemap->layers->head->next) {
			// starting from the second tileset, switch to equal depth test
			if(options.depthTest) {
				glDepthFunc(GL_EQUAL);
			}
		}

		// set this layer as active
		tilemap->activeLayerTexture = layerTexture;

		int tilesetId = 1; // start with one since zero is blank
		for(GList *iter2 = tilemap->tilesets->head; iter2 != NULL; iter2 = iter2->next, tilesetId++) {
			ShovelerMaterialTilemapTileset *tileset = (ShovelerMaterialTilemapTileset *) iter2->data;

			// set uniform values by setting this tileset as active
			tilemap->activeTilesetId = tilesetId;
			tilemap->activeTileset = *tileset;

			if(!shovelerShaderUse(shader)) {
				shovelerLogWarning("Failed to use shader for layer %p of tilemap material %p, scene %p, camera %p, light %p and model %p.", layerTexture, material, scene, camera, light, model);
				return false;
			}

			if(!shovelerModelRender(model)) {
				shovelerLogWarning("Failed to render model %p for layer %p of tilemap material %p in scene %p for camera %p and light %p.", layerTexture, model, material, scene, camera, light);
				return false;
			}
		}
	}

	// reset back to original blending and depth test mode
	if(options.blend) {
		glBlendFunc(options.blendSourceFactor, options.blendDestinationFactor);
	}
	if(options.depthTest) {
		glDepthFunc(options.depthFunction);
	}

	return true;
}

static void freeTilemap(ShovelerMaterial *material)
{
	Tilemap *tilemap = material->data;

	for(GList *iter = tilemap->tilesets->head; iter != NULL; iter = iter->next) {
		free(iter->data);
	}
	g_queue_free(tilemap->tilesets);

	g_queue_free(tilemap->layers);

	free(tilemap->tilemapSampler);
	free(tilemap);
}
