#include <assert.h> // assert
#include <limits.h> // UCHAR_MAX
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

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
	"uniform bool sceneDebugMode;\n"
	"uniform int layerWidth;\n"
	"uniform int layerHeight;\n"
	"uniform int tilesetId;\n"
	"uniform int tilesetColumns;\n"
	"uniform int tilesetRows;\n"
	"uniform int tilesetPadding;\n"
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
	"		vec2 layerSize = textureSize(layer, 0);\n"
	"		vec2 layerInverseSize = 1.0 / layerSize;\n"
	""
	"		vec2 tilemapScaledUv = worldUv * layerSize;\n"
	"		vec2 tileUv = tilemapScaledUv;\n"
	""
	"		// To avoid singularities around 0 and 1, make sure we don't break continuity of the uv function\n"
	"		// because of flooring.\n"
	"		if (worldUv.x > 0.5 * layerInverseSize.x) {\n"
	"			if (worldUv.x < (1.0 - 0.5 * layerInverseSize.x)) {\n"
	"				tileUv.x -= floor(tilemapScaledUv.x);\n"
	"			} else {\n"
	"				tileUv.x -= floor((1.0 - 0.5 * layerInverseSize.x) * layerSize.x);\n"
	"			}\n"
	"		}\n"
	"		if (worldUv.y > 0.5 * layerInverseSize.y) {\n"
	"			if (worldUv.y < (1.0 - 0.5 * layerInverseSize.y)) {\n"
	"				tileUv.y -= floor(tilemapScaledUv.y);\n"
	"			} else {\n"
	"				tileUv.y -= floor((1.0 - 0.5 * layerInverseSize.y) * layerSize.y);\n"
	"			}\n"
	"		}\n"
	""
	"		vec2 tilesetSize = textureSize(tileset, 0);\n"
	"		vec2 tilesetInverseDimensions = 1.0 / vec2(tilesetColumns, tilesetRows);\n"
	"		vec2 paddedTileSize = tilesetSize * tilesetInverseDimensions;\n"
	"		vec2 paddedTilePaddingFraction = vec2(tilesetPadding) / paddedTileSize;\n"
	"		vec2 tilePaddingScaleFactor = vec2(1.0) - 2.0 * paddedTilePaddingFraction;"
	""
	"		vec2 tilePaddedUv = paddedTilePaddingFraction + tilePaddingScaleFactor * tileUv;\n"
	"		vec2 tilesetUv = (tile.xy + tilePaddedUv) * tilesetInverseDimensions;\n"
	""
	"		vec4 color = texture2D(tileset, tilesetUv).rgba;\n"
	"		if (sceneDebugMode) {\n"
	"			fragmentColor = vec4(tilesetUv.xy, tilesetUv.y, 1.0);\n"
	"		} else {\n"
	"			fragmentColor = color;\n"
	"		}\n"
	"	} else {\n"
	"		fragmentColor = vec4(0.0);\n"
	"	}\n"
	"}\n";

typedef struct {
	ShovelerMaterial *material;
	ShovelerSampler *tilemapSampler;
	int activeLayerWidth;
	int activeLayerHeight;
	ShovelerTexture *activeLayerTexture;
	int activeTilesetId;
	int activeTilesetColumns;
	int activeTilesetRows;
	int activeTilesetPadding;
	ShovelerTexture *activeTilesetTexture;
	ShovelerSampler *activeTilesetSampler;
	/** list of (ShovelerTexture *) */
	GQueue *layers;
	/** list of (ShovelerMaterialTilemapTileset *) */
	GQueue *tilesets;
} Tilemap;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeTilemap(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialTilemapCreate()
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	Tilemap *tilemap = malloc(sizeof(Tilemap));
	tilemap->material = shovelerMaterialCreate(program);
	tilemap->material->data = tilemap;
	tilemap->material->render = render;
	tilemap->material->freeData = freeTilemap;
	tilemap->tilemapSampler = shovelerSamplerCreate(false, false, true);
	tilemap->activeLayerWidth = 0;
	tilemap->activeLayerHeight = 0;
	tilemap->activeLayerTexture = NULL;
	tilemap->activeTilesetId = 0;
	tilemap->activeTilesetColumns = 0;
	tilemap->activeTilesetRows = 0;
	tilemap->activeTilesetPadding = 0;
	tilemap->activeTilesetTexture = NULL;
	tilemap->activeTilesetSampler = NULL;
	tilemap->layers = g_queue_new();
	tilemap->tilesets = g_queue_new();

	shovelerUniformMapInsert(tilemap->material->uniforms, "layerWidth", shovelerUniformCreateIntPointer(&tilemap->activeLayerWidth));
	shovelerUniformMapInsert(tilemap->material->uniforms, "layerHeight", shovelerUniformCreateIntPointer(&tilemap->activeLayerHeight));

	shovelerUniformMapInsert(tilemap->material->uniforms, "layer", shovelerUniformCreateTexturePointer(&tilemap->activeLayerTexture, &tilemap->tilemapSampler));

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetId", shovelerUniformCreateIntPointer(&tilemap->activeTilesetId));

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetColumns", shovelerUniformCreateIntPointer(&tilemap->activeTilesetColumns));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetRows", shovelerUniformCreateIntPointer(&tilemap->activeTilesetRows));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetPadding", shovelerUniformCreateIntPointer(&tilemap->activeTilesetPadding));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&tilemap->activeTilesetTexture, &tilemap->activeTilesetSampler));

	return tilemap->material;
}

int shovelerMaterialTilemapAddLayer(ShovelerMaterial *material, ShovelerTexture *layerTexture)
{
	Tilemap *tilemap = material->data;

	g_queue_push_tail(tilemap->layers, layerTexture);
	return g_queue_get_length(tilemap->layers) - 1;
}

int shovelerMaterialTilemapAddTileset(ShovelerMaterial *material, ShovelerTileset *tileset)
{
	Tilemap *tilemap = material->data;

	g_queue_push_tail(tilemap->tilesets, tileset);
	return g_queue_get_length(tilemap->tilesets); // start with one since zero is blank
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	Tilemap *tilemap = material->data;

	// since we are only changing uniform pointer values per layer, we can reuse the same shader for all of them
	ShovelerShader *shader = shovelerSceneGenerateShader(scene, camera, light, model, material, NULL);

	shovelerRenderStateEnableBlend(renderState, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(GList *iter = tilemap->layers->head; iter != NULL; iter = iter->next) {
		ShovelerTexture *layerTexture = (ShovelerTexture *) iter->data;

		if(iter == tilemap->layers->head->next) {
			// starting from the second tileset, switch to equal depth test
			shovelerRenderStateEnableDepthTest(renderState, GL_EQUAL);
		}

		// set this layer as active
		tilemap->activeLayerWidth = layerTexture->image->width;
		tilemap->activeLayerHeight = layerTexture->image->height;
		tilemap->activeLayerTexture = layerTexture;

		int tilesetId = 1; // start with one since zero is blank
		for(GList *iter2 = tilemap->tilesets->head; iter2 != NULL; iter2 = iter2->next, tilesetId++) {
			ShovelerTileset *tileset = (ShovelerTileset *) iter2->data;

			// set uniform values by setting this tileset as active
			tilemap->activeTilesetId = tilesetId;
			tilemap->activeTilesetColumns = tileset->columns;
			tilemap->activeTilesetRows = tileset->rows;
			tilemap->activeTilesetPadding = tileset->padding;
			tilemap->activeTilesetTexture = tileset->texture;
			tilemap->activeTilesetSampler = tileset->sampler;

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

	return true;
}

void shovelerMaterialTilemapSetActiveTiles(ShovelerMaterial *tilemapMaterial, ShovelerTexture *tiles)
{
	Tilemap *tilemap = tilemapMaterial->data;

	tilemap->activeLayerWidth = tiles->image->width;
	tilemap->activeLayerHeight = tiles->image->height;
	tilemap->activeLayerTexture = tiles;
}

void shovelerMaterialTilemapSetActiveTileset(ShovelerMaterial *tilemapMaterial, int tilesetId, ShovelerTileset *tileset)
{
	Tilemap *tilemap = tilemapMaterial->data;

	tilemap->activeTilesetId = tilesetId;
	tilemap->activeTilesetColumns = tileset->columns;
	tilemap->activeTilesetRows = tileset->rows;
	tilemap->activeTilesetPadding = tileset->padding;
	tilemap->activeTilesetTexture = tileset->texture;
	tilemap->activeTilesetSampler = tileset->sampler;
}

static void freeTilemap(ShovelerMaterial *material)
{
	Tilemap *tilemap = material->data;

	g_queue_free(tilemap->tilesets);
	g_queue_free(tilemap->layers);

	free(tilemap->tilemapSampler);
	free(tilemap);
}
