#include <assert.h> // assert
#include <limits.h> // UCHAR_MAX
#include <stdlib.h> // malloc, free

#include "shoveler/material/tilemap.h"
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
	ShovelerMaterialTilemapLayerData activeLayerData;
} Tilemap;

typedef struct {
	ShovelerMaterial *material;
	Tilemap *tilemap;
	ShovelerMaterialTilemapLayerData layerData;
} TilemapLayer;

static int attachTilemapLayerUniforms(ShovelerMaterial *tilemapLayerMaterial, ShovelerShader *shader, void *userData);
static void freeTilemap(ShovelerMaterial *tilemapMaterial);
static void freeTilemapLayer(ShovelerMaterial *tilemapLayerMaterial);

ShovelerMaterial *shovelerMaterialTilemapCreate()
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	Tilemap *tilemap = malloc(sizeof(Tilemap));
	tilemap->material = shovelerMaterialCreate(program);
	tilemap->material->data = tilemap;
	tilemap->material->freeData = freeTilemap;
	tilemap->tilemapSampler = shovelerSamplerCreate(false, true);

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapWidth", shovelerUniformCreateIntPointer(&tilemap->activeLayerData.tilemapWidth));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapHeight", shovelerUniformCreateIntPointer(&tilemap->activeLayerData.tilemapHeight));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetHeight", shovelerUniformCreateIntPointer(&tilemap->activeLayerData.tilesetHeight));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetWidth", shovelerUniformCreateIntPointer(&tilemap->activeLayerData.tilesetWidth));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetId", shovelerUniformCreateIntPointer(&tilemap->activeLayerData.tilesetId));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemap", shovelerUniformCreateTexturePointer(&tilemap->activeLayerData.tilemapTexture, &tilemap->tilemapSampler));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&tilemap->activeLayerData.tilesetTexture, &tilemap->activeLayerData.tilesetSampler));

	return tilemap->material;
}

ShovelerMaterial *shovelerMaterialTilemapCreateLayer(ShovelerMaterial *tilemapMaterial, ShovelerMaterialTilemapLayerData layerData)
{
	assert(layerData.tilemapWidth <= UCHAR_MAX);
	assert(layerData.tilemapHeight <= UCHAR_MAX);
	assert(layerData.tilesetId <= UCHAR_MAX);
	assert(layerData.tilesetWidth <= UCHAR_MAX);
	assert(layerData.tilesetHeight <= UCHAR_MAX);

	TilemapLayer *tilemapLayer = malloc(sizeof(TilemapLayer));
	tilemapLayer->material = shovelerMaterialCreateUnmanaged(tilemapMaterial->program);
	tilemapLayer->material->data = tilemapLayer;
	tilemapLayer->material->attachUniforms = attachTilemapLayerUniforms;
	tilemapLayer->material->freeData = freeTilemapLayer;
	tilemapLayer->tilemap = tilemapMaterial->data;
	tilemapLayer->layerData = layerData;

	return tilemapLayer->material;
}

static int attachTilemapLayerUniforms(ShovelerMaterial *tilemapLayerMaterial, ShovelerShader *shader, void *userData)
{
	TilemapLayer *tilemapLayer = tilemapLayerMaterial->data;
	tilemapLayer->tilemap->activeLayerData = tilemapLayer->layerData;
	return shovelerMaterialAttachUniforms(tilemapLayer->tilemap->material, shader, userData);
}

static void freeTilemap(ShovelerMaterial *tilemapMaterial)
{
	Tilemap *tilemap = tilemapMaterial->data;

	free(tilemap->tilemapSampler);
	free(tilemap);
}

static void freeTilemapLayer(ShovelerMaterial *tilemapLayerMaterial)
{
	TilemapLayer *tilemapLayer = tilemapLayerMaterial->data;
	free(tilemapLayer);
}
