#include <stdlib.h> // malloc, free

#include "material/texture.h"
#include "shader_program.h"

typedef struct {
	ShovelerTexture *texture;
	bool manageTexture;
	ShovelerSampler *sampler;
	bool manageSampler;
} ShovelerMaterialTextureData;

static void freeMaterialTextureData(ShovelerMaterial *material);

static const char *vertexShaderSource =
		"#version 400\n"
		""
		"uniform mat4 model;\n"
		"uniform mat4 modelNormal;\n"
		"uniform mat4 camera;\n"
		"uniform mat4 lightCamera;\n"
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
		"	lightFrustumPosition4 = lightCamera * worldPosition4;\n"
		""
		"	gl_Position = camera * worldPosition4;\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		""
		"uniform vec3 cameraPosition;\n"
		""
		"uniform vec4 lightColor;\n"
		"uniform float lightExponentialShadowFactor;\n"
		"uniform vec3 lightPosition;\n"
		"uniform mat4 lightCamera;\n"
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
		"	float shadowMapDepth = texture2D(shadowMap, lightScreenPosition.xy).r;\n"
		"	float fragmentDepth = lightScreenPosition.z;\n"
		"	float exponentialShadowFactor = getExponentialShadowFactor(shadowMapDepth, fragmentDepth);\n"
		""
		"	vec4 color = vec4(texture2D(textureImage, worldUv).rgb, 1.0);\n"
		"	vec3 lightDirection = normalize(worldPosition - lightPosition);\n"
		"	vec3 normal = normalize(worldNormal);\n"
		"	"
		"	float ambientFactor = 0.2;\n"
		"	float diffuseFactor = clamp(dot(-lightDirection, normal), 0.0, 1.0);\n"
		""
		"	vec3 cameraDirection = normalize(cameraPosition - worldPosition);\n"
		"	vec3 reflectedLight = -reflect(-lightDirection, normal);\n"
		"	float specularFactor = pow(clamp(dot(reflectedLight, cameraDirection), 0.0, 1.0), 250.0);\n"
		""
		"	fragmentColor = clamp(ambientFactor * color + exponentialShadowFactor * diffuseFactor * color + exponentialShadowFactor * specularFactor * lightColor, 0.0, 1.0);\n"
		"}\n";

ShovelerMaterial *shovelerMaterialTextureCreate(ShovelerTexture *texture, bool manageTexture, ShovelerSampler *sampler, bool manageSampler)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, fragmentShaderObject, true);

	ShovelerMaterial *material = shovelerMaterialCreate(program);

	ShovelerMaterialTextureData *materialTextureData = malloc(sizeof(ShovelerMaterialTextureData));
	materialTextureData->texture = texture;
	materialTextureData->manageTexture = manageTexture;
	materialTextureData->sampler = sampler;
	materialTextureData->manageSampler = manageSampler;

	material->freeData = freeMaterialTextureData;
	material->data = materialTextureData;

	shovelerUniformMapInsert(material->uniforms, "textureImage", shovelerUniformCreateTexture(texture, sampler));

	return material;
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
