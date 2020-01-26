#include <assert.h> // assert
#include <limits.h> // UCHAR_MAX
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "shoveler/material/tilemap.h"
#include "shoveler/shader_program/model_vertex.h"
#include "shoveler/camera.h"
#include "shoveler/image.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"
#include "shoveler/shader.h"
#include "shoveler/shader_cache.h"
#include "shoveler/shader_program.h"
#include "shoveler/sprite/tilemap.h"
#include "shoveler/tilemap.h"
#include "shoveler/tileset.h"

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
	"uniform vec2 regionPosition;\n"
	"uniform vec2 regionSize;\n"
	"uniform vec2 spritePosition;\n"
	"uniform vec2 spriteSize;\n"
	"uniform int tilesWidth;\n"
	"uniform int tilesHeight;\n"
	"uniform int tilesetId;\n"
	"uniform int tilesetColumns;\n"
	"uniform int tilesetRows;\n"
	"uniform int tilesetPadding;\n"
	"uniform sampler2D tiles;\n"
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
	"		return;\n"
	"	}\n"
	""
	"	vec3 tile = round(255 * texture2D(tiles, spriteUv).xyz);\n"
	"	int tileTilesetId = int(tile.z);\n"
	""
	"	if (tileTilesetId != tilesetId) {\n"
	"		fragmentColor = vec4(0.0f);\n"
	"		return;\n"
	"	}\n"
	""
	"	vec2 tilesSize = textureSize(tiles, 0);\n"
	"	vec2 tilesInverseSize = 1.0 / tilesSize;\n"
	""
	"	vec2 tilemapScaledUv = spriteUv * tilesSize;\n"
	"	vec2 tileUv = tilemapScaledUv;\n"
	""
	"	// To avoid singularities around 0 and 1, make sure we don't break continuity of the uv function\n"
	"	// because of flooring.\n"
	"	if (spriteUv.x > 0.5 * tilesInverseSize.x) {\n"
	"		if (spriteUv.x < (1.0 - 0.5 * tilesInverseSize.x)) {\n"
	"			tileUv.x -= floor(tilemapScaledUv.x);\n"
	"		} else {\n"
	"			tileUv.x -= floor((1.0 - 0.5 * tilesInverseSize.x) * tilesSize.x);\n"
	"		}\n"
	"	}\n"
	"	if (spriteUv.y > 0.5 * tilesInverseSize.y) {\n"
	"		if (spriteUv.y < (1.0 - 0.5 * tilesInverseSize.y)) {\n"
	"			tileUv.y -= floor(tilemapScaledUv.y);\n"
	"		} else {\n"
	"			tileUv.y -= floor((1.0 - 0.5 * tilesInverseSize.y) * tilesSize.y);\n"
	"		}\n"
	"	}\n"
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
	"	fragmentColor = vec4(tilesetUv.xy, tilesetUv.y, 1.0);\n"
	"	} else {\n"
	"		fragmentColor = color;\n"
	"	}\n"
	"}\n";

typedef struct {
	ShovelerMaterial *material;
	ShovelerSampler *tilesSampler;
	ShovelerVector2 activeRegionPosition;
	ShovelerVector2 activeRegionSize;
	ShovelerVector2 activeSpritePosition;
	ShovelerVector2 activeSpriteSize;
	ShovelerTilemap *activeTilemap;
	int activeLayerWidth;
	int activeLayerHeight;
	ShovelerTexture *activeLayerTexture;
	int activeTilesetId;
	int activeTilesetColumns;
	int activeTilesetRows;
	int activeTilesetPadding;
	ShovelerTexture *activeTilesetTexture;
	ShovelerSampler *activeTilesetSampler;
} MaterialData;

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeTilemap(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialTilemapCreate(ShovelerShaderCache *shaderCache, bool screenspace)
{
	GLuint vertexShaderObject = shovelerShaderProgramModelVertexCreate(screenspace);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	MaterialData *materialData = malloc(sizeof(MaterialData));
	materialData->material = shovelerMaterialCreate(shaderCache, screenspace, program);
	materialData->material->data = materialData;
	materialData->material->render = render;
	materialData->material->freeData = freeTilemap;
	materialData->tilesSampler = shovelerSamplerCreate(false, false, true);
	materialData->activeRegionPosition = shovelerVector2(0.0f, 0.0f);
	materialData->activeRegionSize = shovelerVector2(1.0f, 1.0f);
	materialData->activeSpritePosition = shovelerVector2(0.0f, 0.0f);
	materialData->activeSpriteSize = shovelerVector2(1.0f, 1.0f);
	materialData->activeLayerWidth = 0;
	materialData->activeLayerHeight = 0;
	materialData->activeLayerTexture = NULL;
	materialData->activeTilesetId = 0;
	materialData->activeTilesetColumns = 0;
	materialData->activeTilesetRows = 0;
	materialData->activeTilesetPadding = 0;
	materialData->activeTilesetTexture = NULL;
	materialData->activeTilesetSampler = NULL;

	shovelerUniformMapInsert(materialData->material->uniforms, "regionPosition", shovelerUniformCreateVector2Pointer(&materialData->activeRegionPosition));
	shovelerUniformMapInsert(materialData->material->uniforms, "regionSize", shovelerUniformCreateVector2Pointer(&materialData->activeRegionSize));
	shovelerUniformMapInsert(materialData->material->uniforms, "spritePosition", shovelerUniformCreateVector2Pointer(&materialData->activeSpritePosition));
	shovelerUniformMapInsert(materialData->material->uniforms, "spriteSize", shovelerUniformCreateVector2Pointer(&materialData->activeSpriteSize));

	shovelerUniformMapInsert(materialData->material->uniforms, "tilesWidth", shovelerUniformCreateIntPointer(&materialData->activeLayerWidth));
	shovelerUniformMapInsert(materialData->material->uniforms, "tilesHeight", shovelerUniformCreateIntPointer(&materialData->activeLayerHeight));

	shovelerUniformMapInsert(materialData->material->uniforms, "tiles", shovelerUniformCreateTexturePointer(&materialData->activeLayerTexture, &materialData->tilesSampler));

	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetId", shovelerUniformCreateIntPointer(&materialData->activeTilesetId));

	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetColumns", shovelerUniformCreateIntPointer(&materialData->activeTilesetColumns));
	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetRows", shovelerUniformCreateIntPointer(&materialData->activeTilesetRows));
	shovelerUniformMapInsert(materialData->material->uniforms, "tilesetPadding", shovelerUniformCreateIntPointer(&materialData->activeTilesetPadding));
	shovelerUniformMapInsert(materialData->material->uniforms, "tileset", shovelerUniformCreateTexturePointer(&materialData->activeTilesetTexture, &materialData->activeTilesetSampler));

	return materialData->material;
}

void shovelerMaterialTilemapSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize)
{
	MaterialData *materialData = material->data;

	materialData->activeRegionPosition = regionPosition;
	materialData->activeRegionSize = regionSize;
}

void shovelerMaterialTilemapSetActiveSprite(ShovelerMaterial *material, ShovelerSpriteTilemap *sprite)
{
	MaterialData *materialData = material->data;

	materialData->activeSpritePosition = sprite->sprite.position;
	materialData->activeSpriteSize = sprite->sprite.size;
	materialData->activeTilemap = sprite->tilemap;
}

void shovelerMaterialTilemapSetActive(ShovelerMaterial *tilemapMaterial, ShovelerTilemap *tilemap)
{
	MaterialData *materialData = tilemapMaterial->data;

	materialData->activeTilemap = tilemap;
}

void shovelerMaterialTilemapSetActiveTiles(ShovelerMaterial *tilemapMaterial, ShovelerTexture *tiles)
{
	MaterialData *materialData = tilemapMaterial->data;

	materialData->activeLayerWidth = tiles->image->width;
	materialData->activeLayerHeight = tiles->image->height;
	materialData->activeLayerTexture = tiles;
}

void shovelerMaterialTilemapSetActiveTileset(ShovelerMaterial *tilemapMaterial, int tilesetId, ShovelerTileset *tileset)
{
	MaterialData *materialData = tilemapMaterial->data;

	materialData->activeTilesetId = tilesetId;
	materialData->activeTilesetColumns = tileset->columns;
	materialData->activeTilesetRows = tileset->rows;
	materialData->activeTilesetPadding = tileset->padding;
	materialData->activeTilesetTexture = tileset->texture;
	materialData->activeTilesetSampler = tileset->sampler;
}

static bool render(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	MaterialData *materialData = material->data;

	if(materialData->activeTilemap == NULL) {
		shovelerLogWarning("Failed to render tilemap material %p without an active tilemap.", material);
		return false;
	}

	return shovelerTilemapRender(materialData->activeTilemap, materialData->activeRegionPosition, materialData->activeRegionSize, material, scene, camera, light, model, renderState);
}

static void freeTilemap(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;

	free(materialData->tilesSampler);
	free(materialData);
}
