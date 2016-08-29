#ifndef SHOVELER_SAMPLE_H
#define SHOVELER_SAMPLE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void shovelerSampleInit(GLFWwindow *window, int width, int height, int samples);
void shovelerSampleRender(float dt);
void shovelerSampleTerminate();

#endif
