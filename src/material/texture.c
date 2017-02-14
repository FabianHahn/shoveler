#include <stdlib.h> // malloc, free

#include "material/texture.h"
#include "shader_program.h"

typedef struct {
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
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
		"void main()\n"
		"{\n"
		"	vec3 lightFrustumPosition = lightFrustumPosition4.xyz / lightFrustumPosition4.w;\n"
		"	vec3 lightScreenPosition = 0.5 * (lightFrustumPosition + vec3(1.0, 1.0, 1.0));\n"
		"	float shadowMapDepth = texture2D(shadowMap, lightScreenPosition.xy).r;\n"
		"	float fragmentDepth = lightScreenPosition.z;\n"
		"	float exponentialShadowFactor = clamp(exp(shadowMapDepth * lightExponentialShadowFactor) * exp(-fragmentDepth * lightExponentialShadowFactor), 0.0, 1.0);\n"
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

ShovelerMaterial *shovelerMaterialTextureCreate(ShovelerTexture *texture, ShovelerSampler *sampler)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, fragmentShaderObject, true);

	ShovelerMaterial *material = shovelerMaterialCreate(program);

	ShovelerMaterialTextureData *materialTextureData = malloc(sizeof(ShovelerMaterialTextureData));
	materialTextureData->texture = texture;
	materialTextureData->sampler = sampler;

	material->freeData = freeMaterialTextureData;
	material->data = materialTextureData;

	shovelerMaterialAttachTexture(material, "textureImage", texture, sampler);

	return material;
}

static void freeMaterialTextureData(ShovelerMaterial *material)
{
	ShovelerMaterialTextureData *materialTextureData = material->data;
	shovelerSamplerFree(materialTextureData->sampler);
	shovelerTextureFree(materialTextureData->texture);
	free(materialTextureData);
}