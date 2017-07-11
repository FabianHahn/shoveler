#include "material/particle.h"
#include "shader_program.h"

static const char *vertexShaderSource =
		"#version 400\n"
		"\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"\n"
		"in vec3 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(0, 0, 0, 1.0);\n"
		"	gl_Position = view * worldPosition4;\n"
		"}\n";

static const char *geometryShaderSource =
		"#version 400\n"
		"\n"
		"layout(points) in;\n"
		"layout(triangle_strip, max_vertices = 4) out;\n"
		"\n"
		"uniform mat4 view;\n"\
		"uniform mat4 projection;\n"\
		"uniform float particleSize;\n"
		"\n"
		"out vec2 worldUv;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 viewPosition = gl_in[0].gl_Position;\n"
		"\n"
		"	gl_Position = projection * vec4(viewPosition.xy + particleSize * vec2(-0.5, -0.5), viewPosition.zw);\n"
		"	worldUv = vec2(0.0, 0.0);\n"
		"	EmitVertex();\n"
		"\n"
		"	gl_Position = projection * vec4(viewPosition.xy + particleSize * vec2(0.5, -0.5), viewPosition.zw);\n"
		"	worldUv = vec2(1.0, 0.0);\n"
		"	EmitVertex();\n"
		"\n"
		"	gl_Position = projection * vec4(viewPosition.xy + particleSize * vec2(-0.5, 0.5), viewPosition.zw);\n"
		"	worldUv = vec2(0.0, 1.0);\n"
		"	EmitVertex();\n"
		"\n"
		"	gl_Position = projection * vec4(viewPosition.xy + particleSize * vec2(0.5, 0.5), viewPosition.zw);\n"
		"	worldUv = vec2(1.0, 1.0);\n"
		"	EmitVertex();\n"
		"\n"
		"	EndPrimitive();\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		"\n"
		"uniform vec4 particleColor;\n"
		"\n"
		"in vec2 worldUv;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	fragmentColor = vec4(worldUv, 1.0, 1.0);\n"
		"}\n";

ShovelerMaterial *shovelerMaterialParticleCreate(float size, ShovelerVector4 color)
{
	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint geometryShaderObject = shovelerShaderProgramCompileFromString(geometryShaderSource, GL_GEOMETRY_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, geometryShaderObject, fragmentShaderObject, true);
	ShovelerMaterial *material = shovelerMaterialCreate(program);

	shovelerUniformMapInsert(material->uniforms, "particleSize", shovelerUniformCreateFloat(size));
	shovelerUniformMapInsert(material->uniforms, "particleColor", shovelerUniformCreateVector4(color));

	return material;
}
