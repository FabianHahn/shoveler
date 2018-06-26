#include <assert.h> // assert
#include <limits.h> // UCHAR_MAX
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy
#include <shoveler/material/tilemap.h>

#include "shoveler/material/tilemap.h"
#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/scene.h"
#include "shoveler/shader.h"
#include "shoveler/shader_program.h"

static const char *vertexShaderSource =
		"#version 400\n"
		"\n"
		"uniform int tilemapWidth;\n"
		"uniform int tilemapHeight;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"\n"
		"in vec2 position;\n"
		"in vec2 uv;\n"
		"\n"
		"flat out vec2 fragmentPosition;\n"
		"out vec2 fragmentUv;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(position.x / tilemapWidth, position.y / tilemapHeight, 0.0, 1.0);\n"
		"	fragmentPosition = position;\n"
		"	fragmentUv = uv;\n"
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
		"uniform sampler2D tilemap;\n"
		"uniform sampler2D tileset;\n"
		"\n"
		"flat in vec2 fragmentPosition;\n"
		"in vec2 fragmentUv;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec2 tilemapIndex = fragmentPosition / vec2(tilemapWidth, tilemapHeight);\n"
		"	vec3 tile = round(255 * texture2D(tilemap, tilemapIndex).xyz);\n"
		"	int tileTilesetId = int(tile.z);\n"
		""
		"	if (tileTilesetId == tilesetId) {\n"
		"		vec3 color = texture2D(tileset, tile.xy / vec2(tilesetWidth, tilesetHeight)).rgb;\n"
		"		fragmentColor = vec4(color, 0.0);\n"
		"	} else {\n"
		"		fragmentColor = vec4(0.0);\n"
		"	}\n"
		"}\n";

typedef struct {
	ShovelerMaterial *material;
	ShovelerSampler *tilemapSampler;
	ShovelerMaterialTilemapLayer activeLayer;
	/** list of (ShovelerMaterialTilemapLayer *) */
	GQueue *layers;
} Tilemap;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model);
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
	tilemap->tilemapSampler = shovelerSamplerCreate(false, true);
	tilemap->activeLayer = (ShovelerMaterialTilemapLayer){0, 0, NULL, 0, 0, 0, NULL, NULL};
	tilemap->layers = g_queue_new();

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapWidth", shovelerUniformCreateIntPointer(&tilemap->activeLayer.tilemapWidth));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapHeight", shovelerUniformCreateIntPointer(&tilemap->activeLayer.tilemapHeight));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetHeight", shovelerUniformCreateIntPointer(&tilemap->activeLayer.tilesetHeight));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetWidth", shovelerUniformCreateIntPointer(&tilemap->activeLayer.tilesetWidth));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetId", shovelerUniformCreateIntPointer(&tilemap->activeLayer.tilesetId));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemap", shovelerUniformCreateTexturePointer(&tilemap->activeLayer.tilemapTexture, &tilemap->tilemapSampler));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&tilemap->activeLayer.tilesetTexture, &tilemap->activeLayer.tilesetSampler));

	return tilemap->material;
}

int shovelerMaterialTilemapAddLayer(ShovelerMaterial *material, ShovelerMaterialTilemapLayer layer)
{
	Tilemap *tilemap = material->data;

	assert(layer.tilemapWidth <= UCHAR_MAX);
	assert(layer.tilemapHeight <= UCHAR_MAX);
	assert(layer.tilesetId <= UCHAR_MAX);
	assert(layer.tilesetWidth <= UCHAR_MAX);
	assert(layer.tilesetHeight <= UCHAR_MAX);

	ShovelerMaterialTilemapLayer *layerCopy = malloc(sizeof(ShovelerMaterialTilemapLayer));
	memcpy(layerCopy, &layer, sizeof(ShovelerMaterialTilemapLayer));

	g_queue_push_tail(tilemap->layers, layerCopy);
	return g_queue_get_length(tilemap->layers) - 1;
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model)
{
	Tilemap *tilemap = material->data;

	// since we are only changing uniform pointer values per layer, we can reuse the same shader for all of them
	ShovelerShader *shader = shovelerSceneGenerateShader(scene, camera, light, model, material, NULL);

	for(GList *iter = tilemap->layers->head; iter != NULL; iter = iter->next) {
		ShovelerMaterialTilemapLayer *layer = (ShovelerMaterialTilemapLayer *) iter->data;

		// set uniform values by setting this layer as active
		tilemap->activeLayer = *layer;

		if(!shovelerShaderUse(shader)) {
			shovelerLogWarning("Failed to use shader for layer %p of tilemap material %p, scene %p, camera %p, light %p and model %p.", layer, material, scene, camera, light, model);
			return false;
		}

		if(!shovelerModelRender(model)) {
			shovelerLogWarning("Failed to render model %p for layer %p of tilemap material %p in scene %p for camera %p and light %p.", layer, model, material, scene, camera, light);
			return false;
		}
	}

	return true;
}

static void freeTilemap(ShovelerMaterial *material)
{
	Tilemap *tilemap = material->data;

	for(GList *iter = tilemap->layers->head; iter != NULL; iter = iter->next) {
		free(iter->data);
	}
	g_queue_free(tilemap->layers);

	free(tilemap->tilemapSampler);
	free(tilemap);
}
