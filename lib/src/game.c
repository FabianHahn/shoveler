#include <stdlib.h> // malloc, free

#include <glad/glad.h>

#include "shoveler/game.h"
#include "shoveler/global.h"
#include "shoveler/input.h"
#include "shoveler/opengl.h"

static void exitKeyHandler(ShovelerGame *game, int key, int scancode, int action, int mods);

ShovelerGame *shovelerGameCreate(const char *windowTitle, int width, int height, int samples, bool fullscreen, bool vsync)
{
	ShovelerGame *game = malloc(sizeof(ShovelerGame));

	// request OpenGL4
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	if(fullscreen) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		shovelerLogInfo("Using borderless fullscreen mode on primary monitor '%s'.", glfwGetMonitorName(monitor));

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		width = mode->width;
		height = mode->height;
		game->window = glfwCreateWindow(width, height, windowTitle, monitor, NULL);
	} else {
		shovelerLogInfo("Using windowed mode.");
		game->window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
	}

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

	shovelerInputInit(game);
	shovelerInputAddKeyCallback(game, exitKeyHandler);

	game->framebuffer = shovelerFramebufferCreate(width, height, samples, 4, 8);
	game->lastFrameTime = glfwGetTime();

	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	g_hash_table_insert(global->games, game->window, game);

	return game;
}

ShovelerGame *shovelerGameGetForWindow(GLFWwindow *window)
{
	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	return g_hash_table_lookup(global->games, window);
}

int shovelerGameRenderFrame(ShovelerGame *game)
{
	glfwMakeContextCurrent(game->window);

	double now = glfwGetTime();
	double dt = now - game->lastFrameTime;
	game->lastFrameTime = now;

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

	shovelerInputTerminate(game);
	glfwDestroyWindow(game->window);
	free(game);
}

static void exitKeyHandler(ShovelerGame *game, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		shovelerLogInfo("Escape key pressed, terminating.");
		glfwSetWindowShouldClose(game->window, GLFW_TRUE);
	}
}
