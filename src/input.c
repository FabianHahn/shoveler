#include <glib.h>

#include "input.h"

static void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods);
static void mouseButtonHandler(GLFWwindow *window, int button, int action, int mods);
static void cursorPosHandler(GLFWwindow *window, double xpos, double ypos);
static void scrollHandler(GLFWwindow *window, double xoffset, double yoffset);

static GQueue *keyCallbacks;
static GQueue *mouseButtonCallbacks;
static GQueue *cursorPosCallbacks;
static GQueue *scrollCallbacks;

void shovelerInputInit(GLFWwindow *window)
{
	keyCallbacks = g_queue_new();
	mouseButtonCallbacks = g_queue_new();
	cursorPosCallbacks = g_queue_new();
	scrollCallbacks = g_queue_new();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyHandler);
	glfwSetMouseButtonCallback(window, mouseButtonHandler);
	glfwSetCursorPosCallback(window, cursorPosHandler);
	glfwSetScrollCallback(window, scrollHandler);
}

void shovelerInputTerminate()
{
	g_queue_free(keyCallbacks);
	g_queue_free(mouseButtonCallbacks);
}

void shovelerInputAddKeyCallback(ShovelerInputKeyCallback *keyCallback)
{
	g_queue_push_tail(keyCallbacks, keyCallback);
}

bool shovelerInputRemoveKeyCallback(ShovelerInputKeyCallback *keyCallback)
{
	g_queue_remove(keyCallbacks, keyCallback);
}

void shovelerInputAddMouseButtonCallback(ShovelerInputMouseButtonCallback *mouseButtonCallback)
{
	g_queue_push_tail(mouseButtonCallbacks, mouseButtonCallback);
}

bool shovelerInputRemoveMouseButtonCallback(ShovelerInputMouseButtonCallback *mouseButtonCallback)
{
	g_queue_remove(mouseButtonCallbacks, mouseButtonCallback);
}

void shovelerInputAddCursorPosCallback(ShovelerInputCursorPosCallback *cursorPosCallback)
{
	g_queue_push_tail(cursorPosCallbacks, cursorPosCallback);
}

bool shovelerInputRemoveCursorPosCallback(ShovelerInputCursorPosCallback *cursorPosCallback)
{
	g_queue_remove(cursorPosCallbacks, cursorPosCallback);
}

void shovelerInputAddScrollCallback(ShovelerInputScrollCallback *scrollCallback)
{
	g_queue_push_tail(scrollCallbacks, scrollCallback);
}

bool shovelerInputRemoveScrollCallback(ShovelerInputScrollCallback *scrollCallback)
{
	g_queue_remove(scrollCallbacks, scrollCallback);
}

static void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	for(GList *iter = keyCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputKeyCallback *keyCallback = iter->data;
		keyCallback(key, scancode, action, mods);
	}
}

static void mouseButtonHandler(GLFWwindow *window, int button, int action, int mods)
{
	for(GList *iter = mouseButtonCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputMouseButtonCallback *mouseButtonCallback = iter->data;
		mouseButtonCallback(button, action, mods);
	}
}

static void cursorPosHandler(GLFWwindow *window, double xpos, double ypos)
{
	for(GList *iter = cursorPosCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputCursorPosCallback *cursorPosCallback = iter->data;
		cursorPosCallback(xpos, ypos);
	}
}

static void scrollHandler(GLFWwindow *window, double xoffset, double yoffset)
{
	for(GList *iter = scrollCallbacks->head; iter != NULL; iter = iter->next) {
		ShovelerInputScrollCallback *scrollCallback = iter->data;
		scrollCallback(xoffset, yoffset);
	}
}
