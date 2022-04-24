#include "shoveler/shader_program.h"

#include <glib.h>
#include <stdlib.h> // free

#include "shoveler/file.h"
#include "shoveler/log.h"
#include "shoveler/opengl.h"

GLuint shovelerShaderProgramCompileFromString(const char* source, GLenum type) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char* errorstr = malloc(length * sizeof(char));
    glGetShaderInfoLog(shader, length, NULL, errorstr);
    shovelerLogError("Failed to compile shader: %s.", errorstr);
    free(errorstr);
    return 0;
  }

  if (!shovelerOpenGLCheckSuccess()) {
    return 0;
  }

  return shader;
}

GLuint shovelerShaderProgramCompileFromFile(const char* filename, GLenum type) {
  char* shaderSource = shovelerFileReadString(filename);
  if (shaderSource == NULL) {
    shovelerLogError("Failed to read shader from '%s'.", filename);
    return 0;
  }

  GLuint shader = shovelerShaderProgramCompileFromString(shaderSource, type);
  free(shaderSource);

  return shader;
}

GLuint shovelerShaderProgramLink(
    GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader, bool deleteShaders) {
  GLuint program = glCreateProgram();

  if (vertexShader > 0) {
    glAttachShader(program, vertexShader);
  }
  if (geometryShader > 0) {
    glAttachShader(program, geometryShader);
  }
  if (fragmentShader > 0) {
    glAttachShader(program, fragmentShader);
  }

  glBindAttribLocation(program, SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, "position");
  glBindAttribLocation(program, SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL, "normal");
  glBindAttribLocation(program, SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV, "uv");

  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == 0) {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    char* errorstr = malloc(length * sizeof(char));
    glGetProgramInfoLog(program, length, NULL, errorstr);
    shovelerLogError("Failed to link shader program: %s.", errorstr);
    free(errorstr);
    return 0;
  }

  if (!shovelerOpenGLCheckSuccess()) {
    return 0;
  }

  if (deleteShaders) {
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);
  }

  shovelerLogTrace("Linked shader program %d.", program);

  return program;
}
