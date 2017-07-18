#ifndef SHOVELER_GAME_H
#define SHOVELER_GAME_H

#include <stdbool.h>

#include <glib.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "framebuffer.h"
#include "scene.h"

struct ShovelerGameStruct;

typedef void (ShovelerGameUpdateCallback)(struct ShovelerGameStruct *game, double dt);

typedef struct ShovelerGameStruct {
	GLFWwindow *window;
	GQueue *keyCallbacks;
	GQueue *mouseButtonCallbacks;
	GQueue *cursorPosCallbacks;
	GQueue *scrollCallbacks;
	ShovelerFramebuffer *framebuffer;
	ShovelerScene *scene;
	ShovelerCamera *camera;
	ShovelerGameUpdateCallback *update;
	double lastFrameTime;
} ShovelerGame;

ShovelerGame *shovelerGameCreate(const char *windowTitle, int width, int height, int samples, bool fullscreen, bool vsync);
ShovelerGame *shovelerGameGetForWindow(GLFWwindow *window);
int shovelerGameRenderFrame(ShovelerGame *game);
void shovelerGameFree(ShovelerGame *game);

static inline bool shovelerGameIsRunning(ShovelerGame *game)
{
	return glfwWindowShouldClose(game->window) == 0;
}

static inline void shovelerGameUseContext(ShovelerGame *game)
{
	glfwMakeContextCurrent(game->window);
}

#endif