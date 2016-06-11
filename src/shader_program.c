#include <stdlib.h> // free

#include <glib.h>

#include "log.h"
#include "opengl.h"
#include "shader_program.h"

GLuint shovelerShaderProgramCompileFromString(const char *source, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == 0) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char errorstr[length];
		glGetShaderInfoLog(shader, length, NULL, errorstr);
		shovelerLogError("Failed to compile shader: %s.", errorstr);
		return 0;
	}

	if(!shovelerOpenGLCheckSuccess()) {
		return 0;
	}

	return shader;
}

GLuint shovelerShaderProgramCompileFromFile(const char *filename, GLenum type)
{
	char *shaderSource;
	gsize shaderLength;
	GError *error = NULL;
	if(g_file_get_contents(filename, &shaderSource, &shaderLength, &error)) {
		shovelerLogError("Failed to read shader from '%s': %s.", filename, error->message);
		g_error_free(error);
		return 0;
	}

	GLuint shader = shovelerShaderProgramCompileFromString(shaderSource, type);
	free(shaderSource);

	return shader;
}

GLuint shovelerShaderProgramLink(GLuint vertexShader, GLuint fragmentShader, bool deleteShaders)
{
	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glBindAttribLocation(program, SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, "position");
	glBindAttribLocation(program, SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL, "normal");
	glBindAttribLocation(program, SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV, "uv");

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status == 0) {
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char errorstr[length];
		glGetProgramInfoLog(program, length, NULL, errorstr);
		shovelerLogError("Failed to link shader program: %s.", errorstr);
		return 0;
	}

	if(!shovelerOpenGLCheckSuccess()) {
		return 0;
	}

	if(deleteShaders) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	shovelerLogTrace("Linked shader program %d.", program);

	return program;
}
