#ifndef SHOVELER_OPENGL_H
#define SHOVELER_OPENGL_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include "log.h"

bool shovelerOpenGLCheckSuccessFull(const char *file, int line);
void shovelerOpenGLHandleErrorFull(const char *file, int line, GLenum error);

#define shovelerOpenGLCheckSuccess() shovelerOpenGLCheckSuccessFull(__FILE__, __LINE__)
#define shovelerOpenGLHandleError(ERROR) shovelerOpenGLHandleErrorFull(__FILE__, __LINE__, error)

#endif
