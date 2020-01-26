#include <stdlib.h> // malloc, free

#include "shoveler/material/tile_sprite.h"
#include "shoveler/material.h"
#include "shoveler/shader_program/model_vertex.h"
#include "shoveler/shader_cache.h"
#include "shoveler/shader_program.h"
#include "shoveler/sprite/tile.h"
#include "shoveler/tileset.h"
#include "shoveler/types.h"
#include "shoveler/uniform.h"
#include "shoveler/uniform_map.h"

static const char *fragmentShaderSource =
	"#version 400\n"
	"\n"
	"uniform bool sceneDebugMode;\n"
	"uniform vec2 regionPosition;\n"
	"uniform vec2 regionSize;\n"
	"uniform vec2 spritePosition;\n"
	"uniform vec2 spriteSize;\n"
	"uniform int tilesetColumn;\n"
	"uniform int tilesetRow;\n"
	"uniform int tilesetColumns;\n"
	"uniform int tilesetRows;\n"
	"uniform int tilesetPadding;\n"
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
	""
	"vec2 getSpriteUv()\n"
	"{\n"
	"	vec2 regionCorner = regionPosition - 0.5 * regionSize;\n"
	"	vec2 spriteCorner = spritePosition - 0.5 * spriteSize;\n"
	"	vec2 spriteOffset = spriteCorner - regionCorner;\n"
	"	vec2 spriteOffsetUv = spriteOffset / regionSize;\n"
	"	vec2 spriteTileOffsetUv = worldUv - spriteOffsetUv;\n"
	""
	"	vec2 spriteUvScale = regionSize / spriteSize;\n"
	"	return spriteTileOffsetUv * spriteUvScale;\n"
	"}\n"
	""
	"void main()\n"
	"{\n"
	"	vec2 spriteUv = getSpriteUv();\n"
	""
	"	if (spriteUv.x < 0.0 ||\n"
	"		spriteUv.x > 1.0 ||\n"
	"		spriteUv.y < 0.0 ||\n"
	"		spriteUv.y > 1.0) {\n"
	"		fragmentColor = vec4(0.0f);\n"
	"		return;"
	"	}\n"
	""
	"	vec2 tile = vec2(tilesetColumn, tilesetRow);\n"
	"	vec2 tileUv = clamp(spriteUv, 0.0, 1.0);\n"
	""
	"	vec2 tilesetSize = textureSize(tileset, 0);\n"
	"	vec2 tilesetInverseDimensions = 1.0 / vec2(tilesetColumns, tilesetRows);\n"
	"	vec2 paddedTileSize = tilesetSize * tilesetInverseDimensions;\n"
	"	vec2 paddedTilePaddingFraction = vec2(tilesetPadding) / paddedTileSize;\n"
	"	vec2 tilePaddingScaleFactor = vec2(1.0) - 2.0 * paddedTilePaddingFraction;"
	""
	"	vec2 tilePaddedUv = paddedTilePaddingFraction + tilePaddingScaleFactor * tileUv;\n"
	"	vec2 tilesetUv = (tile.xy + tilePaddedUv) * tilesetInverseDimensions;\n"
	""
	"	vec4 color = texture2D(tileset, tilesetUv).rgba;\n"
	"	if (sceneDebugMode) {\n"
	"		fragmentColor = vec4(tilesetUv.xy, tilesetUv.y, 1.0);\n"
	"	} else {\n"
	"		fragmentColor = color;\n"
	"	}\n"
	"}\n";

typedef struct {
	ShovelerMaterial *material;
	ShovelerVector2 activeRegionPosition;
	ShovelerVector2 activeRegionSize;
	int activeSpriteTilesetColumn;
	int activeSpriteTilesetRow;
	ShovelerVector2 activeSpritePosition;
	ShovelerVector2 activeSpriteSize;
	int activeTilesetColumns;
	int activeTilesetRows;
	int activeTilesetPadding;
	ShovelerTexture *activeTilesetTexture;
	ShovelerSampler *activeTilesetSampler;
} MaterialData;

static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialTileSpriteCreate(ShovelerShaderCache *shaderCache, bool screenspace)
{
	GLuint vertexShaderObject = shovelerShaderProgramModelVertexCreate(screenspace);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	MaterialData *materialData = malloc(sizeof(MaterialData));
	materialData->material = shovelerMaterialCreate(shaderCache, screenspace, program);
	materialData->material->data = materialData;
	materialData->material->freeData = freeMaterialData;
	materialData->activeRegionPosition = shovelerVector2(0.0f, 0.0f);
	materialData->activeRegionSize = shovelerVector2(1.0f, 1.0f);
	materialData->activeSpriteTilesetColumn = 0;
	materialData->activeSpriteTilesetRow = 0;
	materialData->activeSpritePosition = shovelerVector2(0.0f, 0.0f);
	materialData->activeSpriteSize = shovelerVector2(0.0f, 0.0f);
	materialData->activeTilesetColumns = 0;
	materialData->activeTilesetRows = 0;
	materialData->activeTilesetPadding = 0;
	materialData->activeTilesetTexture = NULL;
	materialData->activeTilesetSampler = NULL;

	shovelerUniformMapInsert(materialData->material->uniforms, "regionPosition", shovelerUniformCreateVector2Pointer(&materialData->activeRegionPosition));
	shovelerUniformMapInsert(materialData->material->uniforms, "regionSize", shovelerUniformCreateVector2Pointer(&materialData->activeRegionSize));

	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetColumn", shovelerUniformCreateIntPointer(&materialData->activeSpriteTilesetColumn));
	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetRow", shovelerUniformCreateIntPointer(&materialData->activeSpriteTilesetRow));

	shovelerUniformMapInsert(materialData->material->uniforms, "spritePosition", shovelerUniformCreateVector2Pointer(&materialData->activeSpritePosition));
	shovelerUniformMapInsert(materialData->material->uniforms, "spriteSize", shovelerUniformCreateVector2Pointer(&materialData->activeSpriteSize));

	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetColumns", shovelerUniformCreateIntPointer(&materialData->activeTilesetColumns));
	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetRows", shovelerUniformCreateIntPointer(&materialData->activeTilesetRows));
	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetPadding", shovelerUniformCreateIntPointer(&materialData->activeTilesetPadding));
	shovelerUniformMapInsert(materialData->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&materialData->activeTilesetTexture, &materialData->activeTilesetSampler));

	return materialData->material;
}

void shovelerMaterialTileSpriteSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize)
{
	MaterialData *materialData = material->data;
	materialData->activeRegionPosition = regionPosition;
	materialData->activeRegionSize = regionSize;
}

void shovelerMaterialTileSpriteSetActive(ShovelerMaterial *material, const ShovelerSpriteTile *spriteTile)
{
	MaterialData *materialData = material->data;
	materialData->activeSpriteTilesetColumn = spriteTile->tilesetColumn;
	materialData->activeSpriteTilesetRow = spriteTile->tilesetRow;
	materialData->activeSpritePosition = spriteTile->sprite.position;
	materialData->activeSpriteSize = spriteTile->sprite.size;
	materialData->activeTilesetRows = spriteTile->tileset->rows;
	materialData->activeTilesetColumns = spriteTile->tileset->columns;
	materialData->activeTilesetPadding = spriteTile->tileset->padding;
	materialData->activeTilesetTexture = spriteTile->tileset->texture;
	materialData->activeTilesetSampler = spriteTile->tileset->sampler;
}

static void freeMaterialData(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;
	free(materialData);
}
