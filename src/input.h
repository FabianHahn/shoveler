#ifndef SHOVELER_INPUT_H
#define SHOVELER_INPUT_H

#include <stdbool.h> // bool

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"

typedef void (ShovelerInputKeyCallback)(ShovelerGame *game, int key, int scancode, int action, int mods);
typedef void (ShovelerInputMouseButtonCallback)(ShovelerGame *game, int button, int action, int mods);
typedef void (ShovelerInputCursorPosCallback)(ShovelerGame *game, double xpos, double ypos);
typedef void (ShovelerInputScrollCallback)(ShovelerGame *game, double xoffset, double yoffset);

void shovelerInputInit(ShovelerGame *game);
void shovelerInputTerminate(ShovelerGame *game);
void shovelerInputAddKeyCallback(ShovelerGame *game, ShovelerInputKeyCallback *keyCallback);
bool shovelerInputRemoveKeyCallback(ShovelerGame *game, ShovelerInputKeyCallback *keyCallback);
void shovelerInputAddMouseButtonCallback(ShovelerGame *game, ShovelerInputMouseButtonCallback *mouseButtonCallback);
bool shovelerInputRemoveMouseButtonCallback(ShovelerGame *game, ShovelerInputMouseButtonCallback *mouseButtonCallback);
void shovelerInputAddCursorPosCallback(ShovelerGame *game, ShovelerInputCursorPosCallback *cursorPosCallback);
bool shovelerInputRemoveCursorPosCallback(ShovelerGame *game, ShovelerInputCursorPosCallback *cursorPosCallback);
void shovelerInputAddScrollCallback(ShovelerGame *game, ShovelerInputScrollCallback *scrollCallback);
bool shovelerInputRemoveScrollCallback(ShovelerGame *game, ShovelerInputScrollCallback *scrollCallback);

#endif
