#include <stdlib.h> // malloc, free

#include "shoveler/material/tile_sprite.h"
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
	"uniform float model2dPositionX;\n"
	"uniform float model2dPositionZ;\n"
	"uniform float model2dWidth;\n"
	"uniform float model2dHeight;\n"
	"uniform float spritePositionX;\n"
	"uniform float spritePositionZ;\n"
	"uniform float spriteWidth;\n"
	"uniform float spriteHeight;\n"
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
	"	vec2 model2dPosition = vec2(model2dPositionX, model2dPositionZ);\n"
	"	vec2 model2dSize = vec2(spriteWidth, spriteHeight);\n"
	""
	"	vec2 spritePosition = vec2(spritePositionX, spritePositionZ);\n"
	"	vec2 spriteSize = vec2(spriteWidth, spriteHeight);\n"
	""
	"	vec2 spriteOffset = spritePosition - modelPosition;\n"
	"	vec2 spriteOffsetUv = spriteOffset / model2dSize;\n"
	"	vec2 spriteTileOffsetUv = worldUv - spriteOffsetUv;\n"
	""
	"	vec2 spriteUvScale = model2dSize / spriteSize;\n"
	"	vec2 spriteTileUv = spriteTileOffsetUv * spriteUvScale;\n"
	"	vec2 tileUv = clamp(tileUv, 0.0, 1.0);\n"
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
	ShovelerTileset *tileset;
	ShovelerMaterialTileSpriteConfiguration configuration;
} TileSprite;

ShovelerMaterial *shovelerMaterialTileSpriteCreate(ShovelerTileset *tileset, ShovelerMaterialTileSpriteConfiguration configuration)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	TileSprite *tileSprite = malloc(sizeof(TileSprite));
	tileSprite->material = shovelerMaterialCreate(program);
	tileSprite->material->data = tileSprite;
	tileSprite->tileset = tileset;
	tileSprite->configuration = configuration;

	shovelerUniformMapInsert(tileSprite->material->uniforms, "tilesetColumn", shovelerUniformCreateIntPointer(&tileSprite->configuration.tilesetColumn));
	shovelerUniformMapInsert(tileSprite->material->uniforms, "tilesetRow", shovelerUniformCreateIntPointer(&tileSprite->configuration.tilesetRow));

	shovelerUniformMapInsert(tileSprite->material->uniforms, "spritePositionX", shovelerUniformCreateFloatPointer(&tileSprite->configuration.positionX));
	shovelerUniformMapInsert(tileSprite->material->uniforms, "spritePositionZ", shovelerUniformCreateFloatPointer(&tileSprite->configuration.positionZ));

	shovelerUniformMapInsert(tileSprite->material->uniforms, "spriteWidth", shovelerUniformCreateFloatPointer(&tileSprite->configuration.width));
	shovelerUniformMapInsert(tileSprite->material->uniforms, "spriteHeight", shovelerUniformCreateFloatPointer(&tileSprite->configuration.height));

	shovelerUniformMapInsert(tileSprite->material->uniforms, "tilesetColumns", shovelerUniformCreateInt(tileSprite->tileset->columns));
	shovelerUniformMapInsert(tileSprite->material->uniforms, "tilesetRows", shovelerUniformCreateInt(tileSprite->tileset->columns));
	shovelerUniformMapInsert(tileSprite->material->uniforms, "tilesetPadding", shovelerUniformCreateInt(tileSprite->tileset->padding));
	shovelerUniformMapInsert(tileSprite->material->uniforms, "tileset", shovelerUniformCreateTexture(tileSprite->tileset->texture, tileSprite->tileset->sampler));

	return tileSprite->material;
}
