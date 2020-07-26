#include <stdlib.h> // malloc, free

#include "shoveler/material/texture_sprite.h"
#include "shoveler/material.h"
#include "shoveler/sampler.h"
#include "shoveler/shader_program/model_vertex.h"
#include "shoveler/shader_cache.h"
#include "shoveler/shader_program.h"
#include "shoveler/texture.h"
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
	"uniform bool depthOnly;\n"
	"uniform bool alphaMask;\n"
	"uniform vec4 color;\n"
	"uniform sampler2D texture;\n"
	"\n"
	"in vec3 worldPosition;\n"
	"in vec3 worldNormal;\n"
	"in vec2 worldUv;\n"
	"in vec4 lightFrustumPosition4;\n"
	"\n"
	"out vec4 fragmentColor;\n"
	"\n"
	"vec2 getSpriteUv() {\n"
	"	// Compute the UV coordinate offset of this fragment from the sprite corner.\n"
	"	vec2 regionCorner = regionPosition - 0.5 * regionSize;\n"
	"	vec2 spriteCorner = spritePosition - 0.5 * spriteSize;\n"
	"	vec2 spriteOffset = spriteCorner - regionCorner;\n"
	"	vec2 spriteCornerRegionUv = spriteOffset / regionSize;\n"
	"	vec2 spriteWorldUvOffset = worldUv - spriteCornerRegionUv;\n"
	"\n"
	"	// Compute the UV coordinates of the sprite of this fragment within a hypothetical sprite texture.\n"
	"	vec2 spriteUvScale = regionSize / spriteSize;\n"
	"	vec2 spriteUv = spriteWorldUvOffset * spriteUvScale;\n"
	"	return spriteUv;\n"
	"}\n"
	"\n"
	"void main()\n"
	"{\n"
	"	vec2 spriteUv = getSpriteUv();\n"
	"\n"
	"	if (spriteUv.x < 0.0 ||\n"
	"		spriteUv.x > 1.0 ||\n"
	"		spriteUv.y < 0.0 ||\n"
	"		spriteUv.y > 1.0) {\n"
	"		fragmentColor = vec4(0.0f);\n"
	"		return;\n"
	"	}\n"
	"\n"
	"	if(sceneDebugMode) {\n"
	"		fragmentColor = vec4(spriteUv.xy, spriteUv.y, 1.0);\n"
	"	} else if(depthOnly) {\n"
	"		fragmentColor = vec4(texture2D(texture, spriteUv).r);\n"
	"	} else if(alphaMask) {\n"
	"		fragmentColor = vec4(color.rgb, color.a * texture2D(texture, spriteUv).r);\n"
	"	} else {\n"
	"		fragmentColor = vec4(texture2D(texture, spriteUv).rgb, 1.0);\n"
	"	}\n"
	"}\n";

typedef struct {
	ShovelerMaterial *material;
	ShovelerVector2 activeRegionPosition;
	ShovelerVector2 activeRegionSize;
	ShovelerVector2 activeSpritePosition;
	ShovelerVector2 activeSpriteSize;
	ShovelerVector4 activeColor;
	ShovelerTexture *activeTexture;
	ShovelerSampler *activeSampler;
} MaterialData;

static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialTextureSpriteCreate(ShovelerShaderCache *shaderCache, bool screenspace, ShovelerMaterialTextureSpriteType type)
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
	materialData->activeSpritePosition = shovelerVector2(0.0f, 0.0f);
	materialData->activeSpriteSize = shovelerVector2(0.0f, 0.0f);
	materialData->activeColor = shovelerVector4(0.0f, 0.0f, 0.0f, 0.0f);
	materialData->activeTexture = NULL;
	materialData->activeSampler = NULL;

	shovelerUniformMapInsert(materialData->material->uniforms, "regionPosition", shovelerUniformCreateVector2Pointer(&materialData->activeRegionPosition));
	shovelerUniformMapInsert(materialData->material->uniforms, "regionSize", shovelerUniformCreateVector2Pointer(&materialData->activeRegionSize));

	shovelerUniformMapInsert(materialData->material->uniforms, "spritePosition", shovelerUniformCreateVector2Pointer(&materialData->activeSpritePosition));
	shovelerUniformMapInsert(materialData->material->uniforms, "spriteSize", shovelerUniformCreateVector2Pointer(&materialData->activeSpriteSize));

	shovelerUniformMapInsert(materialData->material->uniforms, "depthOnly", shovelerUniformCreateInt(type == SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_DEPTH));
	shovelerUniformMapInsert(materialData->material->uniforms, "alphaMask", shovelerUniformCreateInt(type == SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALPHA_MASK));
	shovelerUniformMapInsert(materialData->material->uniforms, "color", shovelerUniformCreateVector4Pointer(&materialData->activeColor));
	shovelerUniformMapInsert(materialData->material->uniforms, "texture", shovelerUniformCreateTexturePointer(&materialData->activeTexture, &materialData->activeSampler));

	return materialData->material;
}

void shovelerMaterialTextureSpriteSetActiveRegion(ShovelerMaterial *material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize)
{
	MaterialData *materialData = material->data;
	materialData->activeRegionPosition = regionPosition;
	materialData->activeRegionSize = regionSize;
}

void shovelerMaterialTextureSpriteSetActiveSprite(ShovelerMaterial *material, ShovelerVector2 position, ShovelerVector2 size)
{
	MaterialData *materialData = material->data;
	materialData->activeSpritePosition = position;
	materialData->activeSpriteSize = size;
}

void shovelerMaterialTextureSpriteSetColor(ShovelerMaterial *material, ShovelerVector4 color)
{
	MaterialData *materialData = material->data;
	materialData->activeColor = color;
}

void shovelerMaterialTextureSpriteSetActiveTexture(ShovelerMaterial *material, ShovelerTexture *texture, ShovelerSampler *sampler)
{
	MaterialData *materialData = material->data;
	materialData->activeTexture = texture;
	materialData->activeSampler = sampler;
}

static void freeMaterialData(ShovelerMaterial *material)
{
	MaterialData *materialData = material->data;
	free(materialData);
}
