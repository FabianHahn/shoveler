#ifndef SHOVELER_GAME_H
#define SHOVELER_GAME_H

#include <stdbool.h>

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera.h>
#include <shoveler/controller.h>
#include <shoveler/executor.h>
#include <shoveler/framebuffer.h>
#include <shoveler/input.h>
#include <shoveler/scene.h>
#include <shoveler/view.h>

struct ShovelerGameStruct;

typedef void (ShovelerGameUpdateCallback)(struct ShovelerGameStruct *game, double dt);

typedef struct {
	const char *windowTitle;
	int windowedWidth;
	int windowedHeight;
	int samples;
	bool fullscreen;
	bool vsync;
} ShovelerGameWindowSettings;

typedef struct {
	ShovelerReferenceFrame frame;
	float moveFactor;
	float tiltFactor;
} ShovelerGameControllerSettings;

typedef struct ShovelerGameStruct {
	int windowedWidth;
	int windowedHeight;
	int samples;
	bool fullscreen;
	ShovelerExecutor *updateExecutor;
	GLFWwindow *window;
	ShovelerRenderState renderState;
	ShovelerInput *input;
	ShovelerFramebuffer *framebuffer;
	ShovelerScene *scene;
	ShovelerCamera *camera;
	ShovelerController *controller;
	ShovelerView *view;
	ShovelerGameUpdateCallback *update;
	double lastFrameTime;
	double lastFpsPrintTime;
	int framesSinceLastFpsPrint;
} ShovelerGame;

ShovelerGame *shovelerGameCreate(ShovelerCamera *camera, ShovelerGameUpdateCallback *update, const ShovelerGameWindowSettings *windowSettings, const ShovelerGameControllerSettings *controllerSettings);
ShovelerGame *shovelerGameGetForWindow(GLFWwindow *window);
void shovelerGameToggleFullscreen(ShovelerGame *game);
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
