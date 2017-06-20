#include <stdlib.h> // malloc, free

#include "material/depth_texture_gaussian_filter.h"
#include "shader_program.h"

typedef struct {
	int liftExponential;
	float liftExponentialFactor;
	ShovelerVector2 filterDirection;
} ShovelerMaterialDepthTextureGaussianFilterData;

static void freeMaterialDepthTextureGaussianFilterData(ShovelerMaterial *material);

static const char *vertexShaderSource =
		"#version 400\n"
		""
		"uniform mat4 model;\n"
		""
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec2 uv;\n"
		""
		"out vec2 worldUv;\n"
		""
		"void main()\n"
		"{\n"
		"	vec3 imagePosition = 0.5 * (position + vec3(1.0, 1.0, 0.0));\n"
		"	vec4 worldPosition4 = model * vec4(imagePosition, 1.0);\n"
		"	vec3 worldPosition = worldPosition4.xyz / worldPosition4.w;\n"
		"	worldUv = uv;\n"
		"	gl_Position = vec4(worldPosition, 1.0);\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		""
		"uniform bool liftExponential;\n"
		"uniform float liftExponentialFactor;\n"
		"uniform vec2 filterDirection;\n"
		"uniform vec2 inverseTextureSize;\n"
		"uniform sampler2D textureImage;\n"
		""
		"in vec2 worldUv;\n"
		""
		"out float filteredDepth;\n"
		""
		"float gaussianKernel[9] = float[](0.048297, 0.08393, 0.124548, 0.157829, 0.170793, 0.157829, 0.124548, 0.08393, 0.048297);\n"
		""
		"float getTextureSample(vec2 samplePosition)\n"
		"{\n"
		"	float textureSample = texture2D(textureImage, samplePosition).r;\n"
		""
		"	if(liftExponential) {\n"
		"		return exp(textureSample * liftExponentialFactor);\n"
		"	} else {\n"
		"		return textureSample;\n"
		"	}\n"
		"}\n"
		""
		"void main()\n"
		"{\n"
		"	filteredDepth = 0.0;\n"
		""
		"	for(int i = 0; i < 9; i++) {\n"
		"		int offset = i - 4;\n"
		"		vec2 samplePosition = worldUv + offset * filterDirection * inverseTextureSize;\n"
		"		float textureSample = getTextureSample(samplePosition);\n"
		"		filteredDepth += gaussianKernel[i] * textureSample;\n"
		"	}\n"
		"}\n";

ShovelerMaterial *shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(ShovelerTexture **texturePointer, ShovelerSampler **samplerPointer, int width, int height)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, fragmentShaderObject, true);

	ShovelerMaterial *material = shovelerMaterialCreate(program);

	ShovelerMaterialDepthTextureGaussianFilterData *materialDepthTextureGaussianFilterData = malloc(sizeof(ShovelerMaterialDepthTextureGaussianFilterData));
	material->freeData = freeMaterialDepthTextureGaussianFilterData;
	material->data = materialDepthTextureGaussianFilterData;
	shovelerMaterialDepthTextureGaussianFilterDisableExponentialLifting(material);
	shovelerMaterialDepthTextureGaussianFilterSetDirection(material, true, false);

	shovelerUniformMapInsert(material->uniforms, "liftExponential", shovelerUniformCreateIntPointer(&materialDepthTextureGaussianFilterData->liftExponential));
	shovelerUniformMapInsert(material->uniforms, "liftExponentialFactor", shovelerUniformCreateFloatPointer(&materialDepthTextureGaussianFilterData->liftExponentialFactor));
	shovelerUniformMapInsert(material->uniforms, "filterDirection", shovelerUniformCreateVector2Pointer(&materialDepthTextureGaussianFilterData->filterDirection));
	shovelerUniformMapInsert(material->uniforms, "inverseTextureSize", shovelerUniformCreateVector2(shovelerVector2(1.0f / width, 1.0f / height)));
	shovelerUniformMapInsert(material->uniforms, "textureImage", shovelerUniformCreateTexturePointer(texturePointer, samplerPointer));

	return material;
}

void shovelerMaterialDepthTextureGaussianFilterEnableExponentialLifting(ShovelerMaterial *material, float liftExponentialFactor)
{
	ShovelerMaterialDepthTextureGaussianFilterData *materialDepthTextureGaussianFilterData = material->data;
	materialDepthTextureGaussianFilterData->liftExponential = 1;
	materialDepthTextureGaussianFilterData->liftExponentialFactor = liftExponentialFactor;
}

void shovelerMaterialDepthTextureGaussianFilterDisableExponentialLifting(ShovelerMaterial *material)
{
	ShovelerMaterialDepthTextureGaussianFilterData *materialDepthTextureGaussianFilterData = material->data;
	materialDepthTextureGaussianFilterData->liftExponential = 0;
	materialDepthTextureGaussianFilterData->liftExponentialFactor = 0.0f;
}

void shovelerMaterialDepthTextureGaussianFilterSetDirection(ShovelerMaterial *material, bool filterX, bool filterY)
{
	ShovelerMaterialDepthTextureGaussianFilterData *materialDepthTextureGaussianFilterData = material->data;
	materialDepthTextureGaussianFilterData->filterDirection.values[0] = filterX ? 1 : 0;
	materialDepthTextureGaussianFilterData->filterDirection.values[1] = filterY ? 1 : 0;
}

static void freeMaterialDepthTextureGaussianFilterData(ShovelerMaterial *material)
{
	ShovelerMaterialDepthTextureGaussianFilterData *materialDepthTextureGaussianFilterData = material->data;
	free(materialDepthTextureGaussianFilterData);
}
