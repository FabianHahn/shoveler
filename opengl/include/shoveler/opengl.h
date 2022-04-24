#ifndef SHOVELER_OPENGL_H
#define SHOVELER_OPENGL_H

// glad must be included before glfw3 header, which is not alphabetically ordered
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <shoveler/log.h>
#include <stdbool.h> // bool

bool shovelerOpenGLCheckSuccessFull(const char* file, int line);
void shovelerOpenGLHandleErrorFull(const char* file, int line, GLenum error);

#define shovelerOpenGLCheckSuccess() shovelerOpenGLCheckSuccessFull(__FILE__, __LINE__)
#define shovelerOpenGLHandleError(ERROR) shovelerOpenGLHandleErrorFull(__FILE__, __LINE__, error)

#endif
