#ifndef SHOVELER_INPUT_H
#define SHOVELER_INPUT_H

#include <stdbool.h> // bool

#include <GLFW/glfw3.h>

typedef void (ShovelerInputKeyCallback)(int key, int scancode, int action, int mods);
typedef void (ShovelerInputMouseButtonCallback)(int button, int action, int mods);
typedef void (ShovelerInputCursorPosCallback)(double xpos, double ypos);
typedef void (ShovelerInputScrollCallback)(double xoffset, double yoffset);

void shovelerInputInit(GLFWwindow *window);
void shovelerInputTerminate();
void shovelerInputAddKeyCallback(ShovelerInputKeyCallback *keyCallback);
bool shovelerInputRemoveKeyCallback(ShovelerInputKeyCallback *keyCallback);
void shovelerInputAddMouseButtonCallback(ShovelerInputMouseButtonCallback *mouseButtonCallback);
bool shovelerInputRemoveMouseButtonCallback(ShovelerInputMouseButtonCallback *mouseButtonCallback);
void shovelerInputAddCursorPosCallback(ShovelerInputCursorPosCallback *cursorPosCallback);
bool shovelerInputRemoveCursorPosCallback(ShovelerInputCursorPosCallback *cursorPosCallback);
void shovelerInputAddScrollCallback(ShovelerInputScrollCallback *scrollCallback);
bool shovelerInputRemoveScrollCallback(ShovelerInputScrollCallback *scrollCallback);

#endif
