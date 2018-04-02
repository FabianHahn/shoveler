#include <stdlib.h> // malloc, free

#include <glad/glad.h>

#include "shoveler/game.h"
#include "shoveler/global.h"
#include "shoveler/input.h"
#include "shoveler/opengl.h"

static void exitKeyHandler(ShovelerInput *input, int key, int scancode, int action, int mods, void *unused);
static gint64 elapsedNs(double dt);
static void printFps(void *gamePointer);

ShovelerGame *shovelerGameCreate(const char *windowTitle, int windowedWidth, int windowedHeight, int samples, bool fullscreen, bool vsync)
{
	ShovelerGame *game = malloc(sizeof(ShovelerGame));
	game->windowedWidth = windowedWidth;
	game->windowedHeight = windowedHeight;
	game->samples = samples;
	game->fullscreen = fullscreen;
	game->updateExecutor = shovelerExecutorCreateDirect();

	// request OpenGL4
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	int width = windowedWidth;
	int height = windowedHeight;
	GLFWmonitor *monitor = NULL;
	if(fullscreen) {
		monitor = glfwGetPrimaryMonitor();
		shovelerLogInfo("Using borderless fullscreen mode on primary monitor '%s'.", glfwGetMonitorName(monitor));

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		width = mode->width;
		height = mode->height;
	} else {
		shovelerLogInfo("Using windowed mode.");
	}

	game->window = glfwCreateWindow(width, height, windowTitle, monitor, NULL);
	if(game->window == NULL) {
		shovelerLogError("Failed to create glfw window.");
		free(game);
		return NULL;
	}

	glfwMakeContextCurrent(game->window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		shovelerLogError("Failed to initialize glad.");
		glfwDestroyWindow(game->window);
		free(game);
		return NULL;
	}

	shovelerLogInfo("Opened glfw window with name '%s', using OpenGL driver version '%s' by '%s' and GLSL version '%s'.", windowTitle, glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_SHADING_LANGUAGE_VERSION));

	if(!shovelerOpenGLCheckSuccess()) {
		glfwDestroyWindow(game->window);
		free(game);
		return NULL;
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glfwSwapInterval(vsync ? 1 : 0);

	if(!shovelerOpenGLCheckSuccess()) {
		glfwTerminate();
		glfwDestroyWindow(game->window);
		free(game);
		return NULL;
	}

	game->input = shovelerInputCreate(game);
	shovelerInputAddKeyCallback(game->input, exitKeyHandler, NULL);

	game->framebuffer = shovelerFramebufferCreate(width, height, samples, 4, 8);
	game->lastFrameTime = glfwGetTime();
	game->lastFpsPrintTime = game->lastFrameTime;
	game->framesSinceLastFpsPrint = 0;
	shovelerExecutorSchedulePeriodic(game->updateExecutor, 1000, 1000, printFps, game);

	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	g_hash_table_insert(global->games, game->window, game);

	return game;
}

ShovelerGame *shovelerGameGetForWindow(GLFWwindow *window)
{
	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	return g_hash_table_lookup(global->games, window);
}

void shovelerGameToggleFullscreen(ShovelerGame *game)
{
	if(game->fullscreen) {
		shovelerLogInfo("Switching game %p window to windowed mode.", game);
		glfwSetWindowMonitor(game->window, NULL, 0, 0, game->windowedWidth, game->windowedHeight, GLFW_DONT_CARE);

		shovelerFramebufferFree(game->framebuffer);
		game->framebuffer = shovelerFramebufferCreate(game->windowedWidth, game->windowedHeight, game->samples, 4, 8);
	} else {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		shovelerLogInfo("Switching game %p window to fullscreen mode on primary monitor '%s'.", game, glfwGetMonitorName(monitor));

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(game->window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

		shovelerFramebufferFree(game->framebuffer);
		game->framebuffer = shovelerFramebufferCreate(mode->width, mode->height, game->samples, 4, 8);
	}

	game->fullscreen = !game->fullscreen;
}

int shovelerGameRenderFrame(ShovelerGame *game)
{
	glfwMakeContextCurrent(game->window);

	double now = glfwGetTime();
	double dt = now - game->lastFrameTime;
	game->lastFrameTime = now;
	game->framesSinceLastFpsPrint++;

	shovelerExecutorUpdate(game->updateExecutor, elapsedNs(dt));
	game->update(game, dt);

	int rendered = shovelerSceneRenderFrame(game->scene, game->camera, game->framebuffer);
	shovelerFramebufferBlitToDefault(game->framebuffer);

	glfwSwapBuffers(game->window);
	glfwPollEvents();

	return rendered;
}

void shovelerGameFree(ShovelerGame *game)
{
	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	g_hash_table_remove(global->games, game->window);

	shovelerFramebufferFree(game->framebuffer);

	shovelerInputFree(game->input);
	glfwDestroyWindow(game->window);

	shovelerExecutorFree(game->updateExecutor);
	free(game);
}

static void exitKeyHandler(ShovelerInput *input, int key, int scancode, int action, int mods, void *unused)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		shovelerLogInfo("Escape key pressed, terminating.");
		glfwSetWindowShouldClose(input->game->window, GLFW_TRUE);
	}
}

static gint64 elapsedNs(double dt)
{
	gint64 elapsed = dt * G_USEC_PER_SEC;
	if(elapsed > 0) {
		return elapsed;
	} else {
		return 0;
	}
}

static void printFps(void *gamePointer)
{
	ShovelerGame *game = gamePointer;
	double now = glfwGetTime();
	double secondsSinceLastFpsPrint = now - game->lastFpsPrintTime;

	double fps = game->framesSinceLastFpsPrint / secondsSinceLastFpsPrint;
	shovelerLogInfo("Current FPS: %.1f", fps);

	game->lastFpsPrintTime = now;
	game->framesSinceLastFpsPrint = 0;
}
