#include <stdlib.h> // malloc, free

#include "material/screenspace_texture.h"
#include "shader_program.h"

typedef struct {
	ShovelerTexture *texture;
	bool manageTexture;
	ShovelerSampler *sampler;
	bool manageSampler;
} ShovelerMaterialScreenspaceTextureData;

static void freeMaterialScreenspaceTextureData(ShovelerMaterial *material);

static const char *vertexShaderSource =
		"#version 400\n"
		"\n"
		"uniform mat4 model;\n"
		"\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec2 uv;\n"
		"\n"
		"out vec3 worldPosition;"
		"out vec2 worldUv;"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(position, 1.0);\n"
		"	worldPosition = worldPosition4.xyz / worldPosition4.w;\n"
		"	worldUv = uv;\n"
		"	gl_Position = vec4(worldPosition, 1.0);\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		"\n"
		"uniform bool depthTexture;\n"
		"uniform sampler2D textureImage;\n"
		"\n"
		"in vec3 worldPosition;\n"
		"in vec2 worldUv;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 color;\n"
		"	if(depthTexture) {\n"
		"		color = vec4(texture2D(textureImage, worldUv).r);\n"
		"	} else {\n"
		"		color = vec4(texture2D(textureImage, worldUv).rgb, 1.0);\n"
		"	}\n"
		"\n"
		"	fragmentColor = color;\n"
		"}\n";

ShovelerMaterial *shovelerMaterialScreenspaceTextureCreate(ShovelerTexture *texture, bool manageTexture, bool depthTexture, ShovelerSampler *sampler, bool manageSampler)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, fragmentShaderObject, true);

	ShovelerMaterial *material = shovelerMaterialCreate(program);

	ShovelerMaterialScreenspaceTextureData *materialScreenspaceTextureData = malloc(sizeof(ShovelerMaterialScreenspaceTextureData));
	materialScreenspaceTextureData->texture = texture;
	materialScreenspaceTextureData->manageTexture = manageTexture;
	materialScreenspaceTextureData->sampler = sampler;
	materialScreenspaceTextureData->manageSampler = manageSampler;

	material->freeData = freeMaterialScreenspaceTextureData;
	material->data = materialScreenspaceTextureData;

	shovelerUniformMapInsert(material->uniforms, "depthTexture", shovelerUniformCreateInt(depthTexture ? 1 : 0));
	shovelerUniformMapInsert(material->uniforms, "textureImage", shovelerUniformCreateTexture(texture, sampler));

	return material;
}

static void freeMaterialScreenspaceTextureData(ShovelerMaterial *material)
{
	ShovelerMaterialScreenspaceTextureData *materialScreenspaceTextureData = material->data;

	if(materialScreenspaceTextureData->manageSampler) {
		shovelerSamplerFree(materialScreenspaceTextureData->sampler);
	}

	if(materialScreenspaceTextureData->manageTexture) {
		shovelerTextureFree(materialScreenspaceTextureData->texture);
	}

	free(materialScreenspaceTextureData);
}
