#include <assert.h> // assert
#include <limits.h> // UCHAR_MAX
#include <stdlib.h> // malloc, free

#include "shoveler/material/tilemap.h"
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
		"\n"
		"flat out vec2 uv;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(position.x / tilemapWidth, position.y / tilemapHeight, 0.0, 1.0);\n"
		"	uv = position;\n"
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
		"flat in vec2 uv;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	fragmentColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
		"	/*"
		"	vec3 tile = texture2D(tilemap, uv / vec2(tilemapWidth, tilemapHeight)).xyz;\n"
		"	int tileTilesetId = int(tile.z);\n"
		""
		"	if (tileTilesetId == tilesetId) {\n"
		"		vec3 color = texture2D(tileset, tile.xy / vec2(tilesetWidth, tilesetHeight)).rgb;\n"
		"		fragmentColor = vec4(color, 0.0);\n"
		"	} else {\n"
		"		fragmentColor = vec4(0.0);\n"
		"	}\n"
		"	*/"
		"}\n";

typedef struct {
	ShovelerMaterial *material;
	int tilemapWidth;
	int tilemapHeight;
	int tilesetHeight;
	int tilesetWidth;
	int tilesetId;
	ShovelerTexture *tilemapTexture;
	ShovelerSampler *tilemapSampler;
	ShovelerTexture *tilesetTexture;
	ShovelerSampler *tilesetSampler;
} Tilemap;

static void freeTilemap(ShovelerMaterial *tilemapMaterial);

ShovelerMaterial *shovelerMaterialTilemapCreate()
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	Tilemap *tilemap = malloc(sizeof(Tilemap));
	tilemap->material = shovelerMaterialCreate(program);
	tilemap->material->data = tilemap;
	tilemap->tilemapWidth = 0;
	tilemap->tilemapHeight = 0;
	tilemap->tilesetHeight = 0;
	tilemap->tilesetWidth = 0;
	tilemap->tilemapTexture = NULL;
	tilemap->tilemapSampler = shovelerSamplerCreate(false, true);
	tilemap->tilesetTexture = NULL;
	tilemap->tilesetSampler = NULL;

	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapWidth", shovelerUniformCreateIntPointer(&tilemap->tilemapWidth));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemapHeight", shovelerUniformCreateIntPointer(&tilemap->tilemapHeight));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetHeight", shovelerUniformCreateIntPointer(&tilemap->tilesetHeight));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetWidth", shovelerUniformCreateIntPointer(&tilemap->tilesetWidth));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilesetId", shovelerUniformCreateIntPointer(&tilemap->tilesetId));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tilemap", shovelerUniformCreateTexturePointer(&tilemap->tilemapTexture, &tilemap->tilemapSampler));
	shovelerUniformMapInsert(tilemap->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&tilemap->tilesetTexture, &tilemap->tilesetSampler));

	return tilemap->material;
}

void shovelerMaterialTilemapSetSource(ShovelerMaterial *tilemapMaterial, ShovelerTexture *texture)
{
	assert(texture->image->width <= UCHAR_MAX);
	assert(texture->image->height <= UCHAR_MAX);

	Tilemap *tilemap = tilemapMaterial->data;
	tilemap->tilemapWidth = texture->image->width;
	tilemap->tilemapHeight = texture->image->height;
	tilemap->tilemapTexture = texture;
}

void shovelerMaterialTilemapSetTileset(ShovelerMaterial *tilemapMaterial, unsigned char tilesetId, ShovelerTexture *tilesetTexture, ShovelerSampler *tilesetSampler, unsigned char tilesetWidth, unsigned char tilesetHeight)
{
	Tilemap *tilemap = tilemapMaterial->data;
	tilemap->tilesetId = tilesetId;
	tilemap->tilesetWidth = tilesetWidth;
	tilemap->tilesetHeight = tilesetHeight;
	tilemap->tilesetTexture = tilesetTexture;
	tilemap->tilesetSampler = tilesetSampler;
}

static void freeTilemap(ShovelerMaterial *tilemapMaterial)
{
	Tilemap *tilemap = tilemapMaterial->data;

	free(tilemap->tilemapSampler);
	free(tilemap);
}
