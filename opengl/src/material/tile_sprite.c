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
	"uniform vec2 modelPosition2d;\n"
	"uniform vec2 modelSize2d;\n"
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
	"	vec2 spriteCorner = spritePosition - 0.5 * spriteSize;\n"
	"	vec2 spriteOffset = spriteCorner - modelPosition2d;\n"
	"	vec2 spriteOffsetUv = spriteOffset / modelSize2d;\n"
	"	vec2 spriteTileOffsetUv = worldUv - spriteOffsetUv;\n"
	""
	"	vec2 spriteUvScale = modelSize2d / spriteSize;\n"
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
} TileSpriteData;

ShovelerMaterial *shovelerMaterialTileSpriteCreate()
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	TileSpriteData *tileSpriteData = malloc(sizeof(TileSpriteData));
	tileSpriteData->material = shovelerMaterialCreate(program);
	tileSpriteData->material->data = tileSpriteData;
	tileSpriteData->activeSpriteTilesetColumn = 0;
	tileSpriteData->activeSpriteTilesetRow = 0;
	tileSpriteData->activeSpritePosition = shovelerVector2(0.0f, 0.0f);
	tileSpriteData->activeSpriteSize = shovelerVector2(0.0f, 0.0f);
	tileSpriteData->activeTilesetColumns = 0;
	tileSpriteData->activeTilesetRows = 0;
	tileSpriteData->activeTilesetPadding = 0;
	tileSpriteData->activeTilesetTexture = NULL;
	tileSpriteData->activeTilesetSampler = NULL;

	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "tilesetColumn", shovelerUniformCreateIntPointer(&tileSpriteData->activeSpriteTilesetColumn));
	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "tilesetRow", shovelerUniformCreateIntPointer(&tileSpriteData->activeSpriteTilesetRow));

	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "spritePosition", shovelerUniformCreateVector2Pointer(&tileSpriteData->activeSpritePosition));
	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "spriteSize", shovelerUniformCreateVector2Pointer(&tileSpriteData->activeSpriteSize));

	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "tilesetColumns", shovelerUniformCreateIntPointer(&tileSpriteData->activeTilesetColumns));
	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "tilesetRows", shovelerUniformCreateIntPointer(&tileSpriteData->activeTilesetColumns));
	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "tilesetPadding", shovelerUniformCreateIntPointer(&tileSpriteData->activeTilesetPadding));
	shovelerUniformMapInsert(tileSpriteData->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&tileSpriteData->activeTilesetTexture, &tileSpriteData->activeTilesetSampler));

	return tileSpriteData->material;
}

void shovelerMaterialTileSpriteSetActive(ShovelerMaterial *material, const ShovelerCanvasTileSprite *tileSprite)
{
	TileSpriteData *tileSpriteData = material->data;
	tileSpriteData->activeSpriteTilesetColumn = tileSprite->tilesetColumn;
	tileSpriteData->activeSpriteTilesetRow = tileSprite->tilesetRow;
	tileSpriteData->activeSpritePosition = tileSprite->position;
	tileSpriteData->activeSpriteSize = tileSprite->size;
	tileSpriteData->activeTilesetRows = tileSprite->tileset->rows;
	tileSpriteData->activeTilesetColumns = tileSprite->tileset->columns;
	tileSpriteData->activeTilesetPadding = tileSprite->tileset->padding;
	tileSpriteData->activeTilesetTexture = tileSprite->tileset->texture;
	tileSpriteData->activeTilesetSampler = tileSprite->tileset->sampler;
}
