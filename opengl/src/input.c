#include <stdlib.h> // malloc, free

#include <glib.h>

#include "shoveler/game.h"
#include "shoveler/input.h"
#include "shoveler/log.h"

static void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods);
static void mouseButtonHandler(GLFWwindow *window, int button, int action, int mods);
static void cursorPosHandler(GLFWwindow *window, double xpos, double ypos);
static void scrollHandler(GLFWwindow *window, double xoffset, double yoffset);
static void windowSizeHandler(GLFWwindow *window, int width, int height);
static void windowFocusHandler(GLFWwindow *window, int focused);
static void freeKeyCallback(void *keyCallbackPointer);
static void freeMouseButtonCallback(void *mouseButtonCallbackPointer);
static void freeCursorPositionCallback(void *cursorPositionCallbackPointer);
static void freeScrollCallback(void *scrollCallbackPointer);
static void freeWindowSizeCallback(void *windowSizeCallbackPointer);

ShovelerInput *shovelerInputCreate(ShovelerGame *game)
{
	ShovelerInput *input = malloc(sizeof(ShovelerInput));
	input->game = game;
	input->keyCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeKeyCallback, NULL);
	input->mouseButtonCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeMouseButtonCallback, NULL);
	input->cursorPositionCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeCursorPositionCallback, NULL);
	input->scrollCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeScrollCallback, NULL);
	input->windowSizeCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeWindowSizeCallback, NULL);

	glfwSetKeyCallback(game->window, keyHandler);
	glfwSetMouseButtonCallback(game->window, mouseButtonHandler);
	glfwSetCursorPosCallback(game->window, cursorPosHandler);
	glfwSetScrollCallback(game->window, scrollHandler);
	glfwSetWindowSizeCallback(game->window, windowSizeHandler);
	glfwSetWindowFocusCallback(game->window, windowFocusHandler);

	return input;
}

ShovelerInputKeyCallback *shovelerInputAddKeyCallback(ShovelerInput *input, ShovelerInputKeyCallbackFunction *callbackFunction, void *userData)
{
	ShovelerInputKeyCallback *callback = malloc(sizeof(ShovelerInputKeyCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(input->keyCallbacks, callback);
	return callback;
}

ShovelerInputMouseButtonCallback *shovelerInputAddMouseButtonCallback(ShovelerInput *input, ShovelerInputMouseButtonCallbackFunction *callbackFunction, void *userData)
{
	ShovelerInputMouseButtonCallback *callback = malloc(sizeof(ShovelerInputMouseButtonCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(input->mouseButtonCallbacks, callback);
	return callback;
}

ShovelerInputCursorPositionCallback *shovelerInputAddCursorPosCallback(ShovelerInput *input, ShovelerInputCursorPositionCallbackFunction *callbackFunction, void *userData)
{
	ShovelerInputCursorPositionCallback *callback = malloc(sizeof(ShovelerInputCursorPositionCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(input->cursorPositionCallbacks, callback);
	return callback;
}

ShovelerInputScrollCallback *shovelerInputAddScrollCallback(ShovelerInput *input, ShovelerInputScrollCallbackFunction *callbackFunction, void *userData)
{
	ShovelerInputScrollCallback *callback = malloc(sizeof(ShovelerInputScrollCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(input->scrollCallbacks, callback);
	return callback;
}

ShovelerInputWindowSizeCallback *shovelerInputAddWindowSizeCallback(ShovelerInput *input, ShovelerInputWindowSizeCallbackFunction *callbackFunction, void *userData)
{
	ShovelerInputWindowSizeCallback *callback = malloc(sizeof(ShovelerInputWindowSizeCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(input->windowSizeCallbacks, callback);
	return callback;
}

bool shovelerInputRemoveKeyCallback(ShovelerInput *input, ShovelerInputKeyCallback *callback)
{
	return g_hash_table_remove(input->keyCallbacks, callback);
}

bool shovelerInputRemoveMouseButtonCallback(ShovelerInput *input, ShovelerInputMouseButtonCallback *callback)
{
	return g_hash_table_remove(input->mouseButtonCallbacks, callback);
}

bool shovelerInputRemoveCursorPositionCallback(ShovelerInput *input, ShovelerInputCursorPositionCallback *callback)
{
	return g_hash_table_remove(input->cursorPositionCallbacks, callback);
}

bool shovelerInputRemoveScrollCallback(ShovelerInput *input, ShovelerInputScrollCallback *callback)
{
	return g_hash_table_remove(input->scrollCallbacks, callback);
}

bool shovelerInputRemoveWindowSizeCallback(ShovelerInput *input, ShovelerInputWindowSizeCallback *callback)
{
	return g_hash_table_remove(input->windowSizeCallbacks, callback);
}

void shovelerInputFree(ShovelerInput *input)
{
	glfwSetInputMode(input->game->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetKeyCallback(input->game->window, NULL);
	glfwSetMouseButtonCallback(input->game->window, NULL);
	glfwSetCursorPosCallback(input->game->window, NULL);
	glfwSetScrollCallback(input->game->window, NULL);
	glfwSetWindowSizeCallback(input->game->window, NULL);
	glfwSetWindowFocusCallback(input->game->window, NULL);

	g_hash_table_destroy(input->keyCallbacks);
	g_hash_table_destroy(input->mouseButtonCallbacks);
	g_hash_table_destroy(input->cursorPositionCallbacks);
	g_hash_table_destroy(input->scrollCallbacks);
	g_hash_table_destroy(input->windowSizeCallbacks);
	free(input);
}

static void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	GHashTableIter iter;
	ShovelerInputKeyCallback *callback;
	g_hash_table_iter_init(&iter, game->input->keyCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(game->input, key, scancode, action, mods, callback->userData);
	}

	if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
		shovelerGameToggleFullscreen(game);
	}
}

static void mouseButtonHandler(GLFWwindow *window, int button, int action, int mods)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	GHashTableIter iter;
	ShovelerInputMouseButtonCallback *callback;
	g_hash_table_iter_init(&iter, game->input->mouseButtonCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(game->input, button, action, mods, callback->userData);
	}

	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		shovelerLogInfo("Regaining focus on game %p window, enabling cursor capture.", game);
		glfwSetInputMode(game->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

static void cursorPosHandler(GLFWwindow *window, double xpos, double ypos)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	GHashTableIter iter;
	ShovelerInputCursorPositionCallback *callback;
	g_hash_table_iter_init(&iter, game->input->cursorPositionCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(game->input, xpos, ypos, callback->userData);
	}
}

static void scrollHandler(GLFWwindow *window, double xoffset, double yoffset)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	GHashTableIter iter;
	ShovelerInputScrollCallback *callback;
	g_hash_table_iter_init(&iter, game->input->scrollCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(game->input, xoffset, yoffset, callback->userData);
	}
}

static void windowSizeHandler(GLFWwindow *window, int width, int height)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	if(width < 0 || height < 0) {
		return;
	}

	unsigned int unsignedWidth = width;
	unsigned int unsignedHeight = height;

	GHashTableIter iter;
	ShovelerInputWindowSizeCallback *callback;
	g_hash_table_iter_init(&iter, game->input->windowSizeCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(game->input, unsignedWidth, unsignedHeight, callback->userData);
	}
}

static void windowFocusHandler(GLFWwindow *window, int focused)
{
	ShovelerGame *game = shovelerGameGetForWindow(window);

	if(!focused) {
		shovelerLogInfo("Losing focus on game %p window, disabling cursor capture.", game);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

static void freeKeyCallback(void *keyCallbackPointer)
{
	free(keyCallbackPointer);
}

static void freeMouseButtonCallback(void *mouseButtonCallbackPointer)
{
	free(mouseButtonCallbackPointer);
}

static void freeCursorPositionCallback(void *cursorPositionCallbackPointer)
{
	free(cursorPositionCallbackPointer);
}

static void freeScrollCallback(void *scrollCallbackPointer)
{
	free(scrollCallbackPointer);
}

static void freeWindowSizeCallback(void *windowSizeCallbackPointer)
{
	free(windowSizeCallbackPointer);
}
