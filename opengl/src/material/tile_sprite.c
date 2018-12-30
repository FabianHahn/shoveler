#include <stdlib.h> // malloc, free

#include "shoveler/material/tile_sprite.h"
#include "shoveler/shader_program.h"
#include "shoveler/types.h"

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
	"out vec3 worldPosition;\n"
	"out vec3 worldNormal;\n"
	"out vec2 worldUv;\n"
	"out vec4 lightFrustumPosition4;\n"
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
	"\n"
	"void main()\n"
	"{\n"
	"	vec2 tile = vec2(tilesetColumn, tilesetRow);\n"
	""
	"	vec2 regionCorner = regionPosition - 0.5 * regionSize;\n"
	"	vec2 spriteCorner = spritePosition - 0.5 * spriteSize;\n"
	"	vec2 spriteOffset = spriteCorner - regionCorner;\n"
	"	vec2 spriteOffsetUv = spriteOffset / regionSize;\n"
	"	vec2 spriteTileOffsetUv = worldUv - spriteOffsetUv;\n"
	""
	"	vec2 spriteUvScale = regionSize / spriteSize;\n"
	"	vec2 spriteTileUv = spriteTileOffsetUv * spriteUvScale;\n"
	""
	"	if (spriteTileUv.x >= 0.0 &&\n"
	"		spriteTileUv.x <= 1.0 &&\n"
	"		spriteTileUv.y >= 0.0 &&\n"
	"		spriteTileUv.y <= 1.0) {\n"
	"		vec2 tileUv = clamp(spriteTileUv, 0.0, 1.0);\n"
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

ShovelerMaterial *shovelerMaterialTileSpriteCreate()
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	MaterialData *materialData = malloc(sizeof(MaterialData));
	materialData->material = shovelerMaterialCreate(program);
	materialData->material->data = materialData;
	materialData->material->freeData = freeMaterialData;
	materialData->activeSpriteTilesetColumn = 0;
	materialData->activeSpriteTilesetRow = 0;
	materialData->activeSpritePosition = shovelerVector2(0.0f, 0.0f);
	materialData->activeSpriteSize = shovelerVector2(0.0f, 0.0f);
	materialData->activeTilesetColumns = 0;
	materialData->activeTilesetRows = 0;
	materialData->activeTilesetPadding = 0;
	materialData->activeTilesetTexture = NULL;
	materialData->activeTilesetSampler = NULL;

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

void shovelerMaterialTileSpriteSetActive(ShovelerMaterial *material, const ShovelerCanvasTileSprite *tileSprite)
{
	MaterialData *materialData = material->data;
	materialData->activeSpriteTilesetColumn = tileSprite->tilesetColumn;
	materialData->activeSpriteTilesetRow = tileSprite->tilesetRow;
	materialData->activeSpritePosition = tileSprite->position;
	materialData->activeSpriteSize = tileSprite->size;
	materialData->activeTilesetRows = tileSprite->tileset->rows;
	materialData->activeTilesetColumns = tileSprite->tileset->columns;
	materialData->activeTilesetPadding = tileSprite->tileset->padding;
	materialData->activeTilesetTexture = tileSprite->tileset->texture;
	materialData->activeTilesetSampler = tileSprite->tileset->sampler;
}

static void freeMaterialData(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;
	free(materialData);
}
