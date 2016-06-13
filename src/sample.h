#ifndef SHOVELER_SAMPLE_H
#define SHOVELER_SAMPLE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void shovelerSampleInit(GLFWwindow *window, int width, int height);
void shovelerSampleRender(float dt);
void shovelerSampleTerminate();

#endif
