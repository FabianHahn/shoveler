#include "material/depth.h"
#include "shader_program.h"

static const char *vertexShaderSource =
		"#version 400\n"
		"\n"
		"uniform mat4 model;\n"
		"uniform mat4 modelNormal;\n"
		"uniform mat4 camera;\n"
		"\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec2 uv;\n"
		"\n"
		"out vec3 worldPosition;"
		"out vec3 worldNormal;"
		"out vec2 worldUv;"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(position, 1.0);\n"
		"	vec4 worldNormal4 = modelNormal * vec4(normal, 1.0);\n"
		"	worldPosition = worldPosition4.xyz / worldPosition4.w;\n"
		"	worldNormal = worldNormal4.xyz / worldNormal4.w;\n"
		"	worldUv = uv;\n"
		"	gl_Position = camera * worldPosition4;\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		"\n"
		"uniform vec3 lightDirection;\n"
		"uniform vec4 lightColor;\n"
		"uniform vec3 cameraPosition;\n"
		"uniform vec4 color;\n"
		"\n"
		"in vec3 worldPosition;\n"
		"in vec3 worldNormal;\n"
		"in vec2 worldUv;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec3 normal = normalize(worldNormal);\n"
		"	"
		"	float ambientFactor = 0.2;\n"
		"	float diffuseFactor = clamp(dot(-lightDirection, normal), 0.0, 1.0);\n"
		""
		"	vec3 cameraDirection = normalize(cameraPosition - worldPosition);\n"
		"	vec3 reflectedLight = -reflect(-lightDirection, normal);\n"
		"	float specularFactor = pow(clamp(dot(reflectedLight, cameraDirection), 0.0, 1.0), 250.0);\n"
		""
		"	fragmentColor = clamp(ambientFactor * color + diffuseFactor * color + specularFactor * lightColor, 0.0, 1.0);\n"
		"}\n";

ShovelerMaterial *shovelerMaterialColorCreate(ShovelerVector4 color)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, fragmentShaderObject, true);

	ShovelerMaterial *material = shovelerMaterialCreate(program);

	shovelerUniformMapInsert(material->uniforms, "color", shovelerUniformCreateVector4(color));

	return material;
}
