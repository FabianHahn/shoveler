#include "shoveler/material/depth.h"
#include "shoveler/shader_program.h"

static const char *vertexShaderSource =
		"#version 400\n"
		"\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"\n"
		"in vec3 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(position, 1.0);\n"
		"	gl_Position = projection * view * worldPosition4;\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		"\n"
		"out vec4 fragmentDepth;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	fragmentDepth = vec4(gl_FragCoord.z);\n"
		"}\n";

ShovelerMaterial *shovelerMaterialDepthCreate()
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);
	return shovelerMaterialCreate(program);
}
