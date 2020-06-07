#include <stdlib.h> // malloc, free

#include "shoveler/material/texture.h"
#include "shoveler/shader_program/model_vertex.h"
#include "shoveler/shader_cache.h"
#include "shoveler/shader_program.h"
#include "shoveler/uniform.h"
#include "shoveler/uniform_map.h"

typedef struct {
	/** color in alpha mask mode */
	ShovelerVector4 color;
	ShovelerTexture *texture;
	bool manageTexture;
	ShovelerSampler *sampler;
	bool manageSampler;
} ShovelerMaterialTextureData;

static void freeMaterialTextureData(ShovelerMaterial *material);

static const char *fragmentShaderSourceColor =
		"#version 400\n"
		"\n"
		"uniform bool sceneDebugMode;\n"
		"uniform bool depthOnly;\n"
		"uniform bool alphaMask;\n"
		"uniform vec4 color;\n"
		"uniform sampler2D textureImage;\n"
		"\n"
		"in vec3 worldPosition;\n"
		"in vec2 worldUv;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	if(sceneDebugMode) {\n"
		"		fragmentColor = vec4(worldUv.xy, worldUv.y, 1.0);\n"
		"	} else if(depthOnly) {\n"
		"		fragmentColor = vec4(texture2D(textureImage, worldUv).r);\n"
		"	} else if(alphaMask) {\n"
		"		fragmentColor = vec4(color.rgb, color.a * texture2D(textureImage, worldUv).r);\n"
		"	} else {\n"
		"		fragmentColor = vec4(texture2D(textureImage, worldUv).rgb, 1.0);\n"
		"	}\n"
		"}\n";

static const char *fragmentShaderSourcePhong =
		"#version 400\n"
		""
		"uniform vec3 cameraPosition;\n"
		""
		"uniform bool sceneDebugMode;\n"
		"uniform vec3 lightColor;\n"
		"uniform float lightAmbientFactor;\n"
		"uniform float lightExponentialShadowFactor;\n"
		"uniform vec3 lightPosition;\n"
		"uniform bool isExponentialLiftedShadowMap;\n"
		"uniform sampler2D shadowMap;\n"
		""
		"uniform sampler2D textureImage;\n"
		""
		"in vec3 worldPosition;\n"
		"in vec3 worldNormal;\n"
		"in vec2 worldUv;\n"
		"in vec4 lightFrustumPosition4;\n"
		""
		"out vec4 fragmentColor;\n"
		""
		"bool isInLightCamera(vec3 lightScreenPosition)\n"
		"{\n"
		"	return lightScreenPosition.x >= 0.0 && lightScreenPosition.x <= 1.0 && lightScreenPosition.y >= 0.0 && lightScreenPosition.y <= 1.0;\n"
		"}\n"
		""
		"float getExponentialShadowFactor(float shadowMapDepth, float fragmentDepth)\n"
		"{\n"
		"	if(isExponentialLiftedShadowMap) {\n"
		"		return clamp(shadowMapDepth * exp(-fragmentDepth * lightExponentialShadowFactor), 0.0, 1.0);\n"
		"	} else {\n"
		"		return clamp(exp(shadowMapDepth * lightExponentialShadowFactor) * exp(-fragmentDepth * lightExponentialShadowFactor), 0.0, 1.0);\n"
		"	}\n"
		"}\n"
		""
		"void main()\n"
		"{\n"
		"	vec3 lightFrustumPosition = lightFrustumPosition4.xyz / lightFrustumPosition4.w;\n"
		"	vec3 lightScreenPosition = 0.5 * (lightFrustumPosition + vec3(1.0, 1.0, 1.0));\n"
		"	float exponentialShadowFactor = 0.0;\n"
		"	if(isInLightCamera(lightScreenPosition)) {\n"
		"		float shadowMapDepth = texture2D(shadowMap, lightScreenPosition.xy).r;\n"
		"		float fragmentDepth = lightScreenPosition.z;\n"
		"		exponentialShadowFactor = getExponentialShadowFactor(shadowMapDepth, fragmentDepth);\n"
		"	}\n"
		""
		"	vec3 color = texture2D(textureImage, worldUv).rgb;\n"
		"	vec3 lightDirection = normalize(worldPosition - lightPosition);\n"
		"	vec3 normal = normalize(worldNormal);\n"
		"	"
		"	float diffuseFactor = clamp(dot(-lightDirection, normal), 0.0, 1.0);\n"
		""
		"	vec3 cameraDirection = normalize(cameraPosition - worldPosition);\n"
		"	vec3 reflectedLight = -reflect(-lightDirection, normal);\n"
		"	float specularFactor = pow(clamp(dot(reflectedLight, cameraDirection), 0.0, 1.0), 250.0);\n"
		""
		"	if(sceneDebugMode) {\n"
		"		fragmentColor = vec4(worldUv.xy, worldUv.y, 1.0);\n"
		"	} else {\n"
		"		fragmentColor = vec4(clamp(lightAmbientFactor * color + exponentialShadowFactor * diffuseFactor * color * lightColor + exponentialShadowFactor * specularFactor * lightColor, 0.0, 1.0), 1.0);\n"
		"	}\n"
		"}\n";

ShovelerMaterial *shovelerMaterialTextureCreate(ShovelerShaderCache *shaderCache, bool screenspace, ShovelerMaterialTextureType type, ShovelerTexture *texture, bool manageTexture, ShovelerSampler *sampler, bool manageSampler)
{
	GLuint vertexShaderObject = shovelerShaderProgramModelVertexCreate(screenspace);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(
		type == SHOVELER_MATERIAL_TEXTURE_TYPE_PHONG ? fragmentShaderSourcePhong : fragmentShaderSourceColor,
		GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

	ShovelerMaterial *material = shovelerMaterialCreate(shaderCache, screenspace, program);

	ShovelerMaterialTextureData *materialTextureData = malloc(sizeof(ShovelerMaterialTextureData));
	materialTextureData->color = shovelerVector4(0.0f, 0.0f, 0.0f, 0.0f);
	materialTextureData->texture = texture;
	materialTextureData->manageTexture = manageTexture;
	materialTextureData->sampler = sampler;
	materialTextureData->manageSampler = manageSampler;

	material->freeData = freeMaterialTextureData;
	material->data = materialTextureData;

	shovelerUniformMapInsert(material->uniforms, "color", shovelerUniformCreateVector4Pointer(&materialTextureData->color));
	shovelerUniformMapInsert(material->uniforms, "depthOnly", shovelerUniformCreateInt(type == SHOVELER_MATERIAL_TEXTURE_TYPE_DEPTH));
	shovelerUniformMapInsert(material->uniforms, "alphaMask", shovelerUniformCreateInt(type == SHOVELER_MATERIAL_TEXTURE_TYPE_ALPHA_MASK));
	shovelerUniformMapInsert(material->uniforms, "textureImage", shovelerUniformCreateTexture(texture, sampler));

	return material;
}

void shovelerMaterialTextureSetAlphaMaskColor(ShovelerMaterial *material, ShovelerVector4 color)
{
	ShovelerMaterialTextureData *materialTextureData = material->data;
	materialTextureData->color = color;
}

static void freeMaterialTextureData(ShovelerMaterial *material)
{
	ShovelerMaterialTextureData *materialTextureData = material->data;

	if(materialTextureData->manageSampler) {
		shovelerSamplerFree(materialTextureData->sampler);
	}
	if(materialTextureData->manageTexture) {
		shovelerTextureFree(materialTextureData->texture);
	}
	free(materialTextureData);
}
