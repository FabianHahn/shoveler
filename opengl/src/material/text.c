#include <stdlib.h> // malloc free

#include "shoveler/material/text.h"
#include "shoveler/font_atlas.h"
#include "shoveler/font_atlas_texture.h"
#include "shoveler/sampler.h"
#include "shoveler/shader_cache.h"
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
	"uniform int textWidth;\n"
	"uniform int textHeight;\n"
	"uniform int characterMinX;\n"
	"uniform int characterMinY;\n"
	"uniform int characterWidth;\n"
	"uniform int characterHeight;\n"
	"uniform int glyphMinX;\n"
	"uniform int glyphMinY;\n"
	"uniform int glyphWidth;\n"
	"uniform int glyphHeight;\n"
	"uniform int glyphBearingX;\n"
	"uniform int glyphBearingY;\n"
	"uniform bool glyphIsRotated;\n"
	"uniform sampler2D fontAtlas;\n"
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
	"	vec3 tile = round(255 * texture2D(tiles, worldUv).xyz);\n"
	"	int tileTilesetId = int(tile.z);\n"
	""
	"	if (tileTilesetId == tilesetId) {\n"
	"		vec2 tilesSize = textureSize(tiles, 0);\n"
	"		vec2 tilesInverseSize = 1.0 / tilesSize;\n"
	""
	"		vec2 tilemapScaledUv = worldUv * tilesSize;\n"
	"		vec2 tileUv = tilemapScaledUv;\n"
	""
	"		// To avoid singularities around 0 and 1, make sure we don't break continuity of the uv function\n"
	"		// because of flooring.\n"
	"		if (worldUv.x > 0.5 * tilesInverseSize.x) {\n"
	"			if (worldUv.x < (1.0 - 0.5 * tilesInverseSize.x)) {\n"
	"				tileUv.x -= floor(tilemapScaledUv.x);\n"
	"			} else {\n"
	"				tileUv.x -= floor((1.0 - 0.5 * tilesInverseSize.x) * tilesSize.x);\n"
	"			}\n"
	"		}\n"
	"		if (worldUv.y > 0.5 * tilesInverseSize.y) {\n"
	"			if (worldUv.y < (1.0 - 0.5 * tilesInverseSize.y)) {\n"
	"				tileUv.y -= floor(tilemapScaledUv.y);\n"
	"			} else {\n"
	"				tileUv.y -= floor((1.0 - 0.5 * tilesInverseSize.y) * tilesSize.y);\n"
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
	ShovelerSampler *sampler;
	ShovelerFontAtlasTexture *activeFontAtlasTexture;
	int activeTextWidth;
	int activeTextHeight;
	int activeCharacterMinX;
	int activeCharacterMinY;
	int activeCharacterWidth;
	int activeCharacterHeight;
	int activeGlyphMinX;
	int activeGlyphMinY;
	int activeGlyphWidth;
	int activeGlyphHeight;
	int activeGlyphBearingX;
	int activeGlyphBearingY;
	int activeGlyphIsRotated;
	ShovelerTexture *activeTexture;
} MaterialData;

static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialTextCreate(ShovelerShaderCache *shaderCache)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	MaterialData *materialData = malloc(sizeof(MaterialData));
	materialData->material = shovelerMaterialCreate(shaderCache, program);
	materialData->material->data = materialData;
	materialData->material->freeData = freeMaterialData;
	materialData->sampler = shovelerSamplerCreate(false, false, false);
	materialData->activeFontAtlasTexture = NULL;
	materialData->activeTextWidth = 0;
	materialData->activeTextHeight = 0;
	materialData->activeCharacterMinX = 0;
	materialData->activeCharacterMinY = 0;
	materialData->activeCharacterWidth = 0;
	materialData->activeCharacterHeight = 0;
	materialData->activeGlyphMinX = 0;
	materialData->activeGlyphMinY = 0;
	materialData->activeGlyphWidth = 0;
	materialData->activeGlyphHeight = 0;
	materialData->activeGlyphBearingX = 0;
	materialData->activeGlyphBearingY = 0;
	materialData->activeGlyphIsRotated = 0;

	shovelerUniformMapInsert(materialData->material->uniforms, "textWidth", shovelerUniformCreateIntPointer(&materialData->activeTextWidth));
	shovelerUniformMapInsert(materialData->material->uniforms, "textHeight", shovelerUniformCreateIntPointer(&materialData->activeTextHeight));

	shovelerUniformMapInsert(materialData->material->uniforms, "characterMinX", shovelerUniformCreateIntPointer(&materialData->activeCharacterMinX));
	shovelerUniformMapInsert(materialData->material->uniforms, "characterMinY", shovelerUniformCreateIntPointer(&materialData->activeCharacterMinY));
	shovelerUniformMapInsert(materialData->material->uniforms, "characterWidth", shovelerUniformCreateIntPointer(&materialData->activeCharacterWidth));
	shovelerUniformMapInsert(materialData->material->uniforms, "characterHeight", shovelerUniformCreateIntPointer(&materialData->activeCharacterHeight));

	shovelerUniformMapInsert(materialData->material->uniforms, "glyphMinX", shovelerUniformCreateIntPointer(&materialData->activeGlyphMinX));
	shovelerUniformMapInsert(materialData->material->uniforms, "glyphMinY", shovelerUniformCreateIntPointer(&materialData->activeGlyphMinY));
	shovelerUniformMapInsert(materialData->material->uniforms, "glyphWidth", shovelerUniformCreateIntPointer(&materialData->activeGlyphWidth));
	shovelerUniformMapInsert(materialData->material->uniforms, "glyphHeight", shovelerUniformCreateIntPointer(&materialData->activeGlyphHeight));
	shovelerUniformMapInsert(materialData->material->uniforms, "glyphBearingX", shovelerUniformCreateIntPointer(&materialData->activeGlyphBearingX));
	shovelerUniformMapInsert(materialData->material->uniforms, "glyphBearingY", shovelerUniformCreateIntPointer(&materialData->activeGlyphBearingY));
	shovelerUniformMapInsert(materialData->material->uniforms, "glyphIsRotated", shovelerUniformCreateIntPointer(&materialData->activeGlyphIsRotated));

	shovelerUniformMapInsert(materialData->material->uniforms, "fontAtlas", shovelerUniformCreateTexturePointer(&materialData->activeTexture, &materialData->sampler));

	return materialData->material;
}

void shovelerMaterialTextSetActiveFontAtlasTexture(ShovelerMaterial *material, ShovelerFontAtlasTexture *fontAtlasTexture)
{
	MaterialData *materialData = material->data;
	materialData->activeFontAtlasTexture = fontAtlasTexture;
}

static void freeMaterialData(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;

	shovelerSamplerFree(materialData->sampler);
	free(materialData);
}
