#ifndef SHOVELER_SHADER_PROGRAM_H
#define SHOVELER_SHADER_PROGRAM_H

#include <stdbool.h> // bool

#include <glad/glad.h>

typedef enum {
	SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION = 0,
	SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL = 1,
	SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV = 2
} ShovelerShaderProgramAttribute;

GLuint shovelerShaderProgramCompileFromString(const char *source, GLenum type);
GLuint shovelerShaderProgramCompileFromFile(const char *filename, GLenum type);
GLuint shovelerShaderProgramLink(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader, bool deleteShaders);

#endif
