#include <glib.h>

#include "shoveler/input.h"

static void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods);
static void mouseButtonHandler(GLFWwindow *window, int button, int action, int mods);
static void cursorPosHandler(GLFWwindow *window, double xpos, double ypos);
static void scrollHandler(GLFWwindow *window, double xoffset, double yoffset);
static void windowFocusHandler(GLFWwindow *window, int focused);

void shovelerInputInit(ShovelerGame *game)
{
	game->keyCallbacks = g_queue_new();
	game->mouseButtonCallbacks = g_queue_new();
	game->cursorPosCallbacks = g_queue_new();
	game->scrollCallbacks = g_queue_new();

	glfwSetInputMode(game->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(game->window, keyHandler);
	glfwSetMouseButtonCallback(game->window, mouseButtonHandler);
	glfwSetCursorPosCallback(game->window, cursorPosHandler);
	glfwSetScrollCallback(game->window, scrollHandler);
	glfwSetWindowFocusCallback(game->window, windowFocusHandler);
}

void shovelerInputTerminate(ShovelerGame *game)
{
	g_queue_free(game->keyCallbacks);
	g_queue_free(game->mouseButtonCallbacks);
	g_queue_free(game->cursorPosCallbacks);
	g_queue_free(game->scrollCallbacks);
}

void shovelerInputAddKeyCallback(ShovelerGame *game, ShovelerInputKeyCallback *keyCallback)
{
	g_queue_push_tail(game->keyCallbacks, keyCallback);
}

bool shovelerInputRemoveKeyCallback(ShovelerGame *game, ShovelerInputKeyCallback *keyCallback)
{
	return g_queue_remove(game->keyCallbacks, keyCallback);
}

void shovelerInputAddMouseButtonCallback(ShovelerGame *game, ShovelerInputMouseButtonCallback *mouseButtonCallback)
{
	g_queue_push_tail(game->mouseButtonCallbacks, mouseButtonCallback);
}

bool shovelerInputRemoveMouseButtonCallback(ShovelerGame *game, ShovelerInputMouseButtonCallback *mouseButtonCallback)
{
	return g_queue_remove(game->mouseButtonCallbacks, mouseButtonCallback);
}

void shovelerInputAddCursorPosCallback(ShovelerGame *game, ShovelerInputCursorPosCallback *cursorPosCallback)
{
	g_queue_push_tail(game->cursorPosCallbacks, cursorPosCallback);
}

bool shovelerInputRemoveCursorPosCallback(ShovelerGame *game, ShovelerInputCursorPosCallback *cursorPosCallback)
{
	return g_queue_remove(game->cursorPosCallbacks, cursorPosCallback);
}

void shovelerInputAddScrollCallback(ShovelerGame *game, ShovelerInputScrollCallback *scrollCallback)
{
	g_queue_push_tail(game->scrollCallbacks, scrollCallback);
}

bool shovelerInputRemoveScrollCallback(ShovelerGame *game, ShovelerInputScrollCallback *scrollCallback)
{
	return g_queue_remove(game->scrollCallbacks, scrollCallback);
}

static void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	for(GList *iter = game->keyCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputKeyCallback *keyCallback = iter->data;
		keyCallback(game, key, scancode, action, mods);
	}
}

static void mouseButtonHandler(GLFWwindow *window, int button, int action, int mods)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	for(GList *iter = game->mouseButtonCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputMouseButtonCallback *mouseButtonCallback = iter->data;
		mouseButtonCallback(game, button, action, mods);
	}
}

static void cursorPosHandler(GLFWwindow *window, double xpos, double ypos)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	for(GList *iter = game->cursorPosCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputCursorPosCallback *cursorPosCallback = iter->data;
		cursorPosCallback(game, xpos, ypos);
	}
}

static void scrollHandler(GLFWwindow *window, double xoffset, double yoffset)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	for(GList *iter = game->scrollCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputScrollCallback *scrollCallback = iter->data;
		scrollCallback(game, xoffset, yoffset);
	}
}

static void windowFocusHandler(GLFWwindow *window, int focused)
{
	if(focused) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	} else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
