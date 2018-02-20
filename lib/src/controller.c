#include <stdlib.h> // malloc, free

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shoveler/controller.h"

static void triggerTilt(ShovelerController *controller, ShovelerVector2 tiltAmount);
static void triggerMove(ShovelerController *controller, ShovelerVector3 moveAmount);
static void freeTiltCallback(void *tiltCallbackPointer);
static void freeMoveCallback(void *moveCallbackPointer);

ShovelerController *shovelerControllerCreate(ShovelerGame *game, float moveFactor, float tiltFactor)
{
	ShovelerController *controller = malloc(sizeof(ShovelerController));
	controller->game = game;
	controller->moveFactor = moveFactor;
	controller->tiltFactor = tiltFactor;

	glfwGetCursorPos(game->window, &controller->previousCursorX, &controller->previousCursorY);

	controller->tiltCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeTiltCallback, NULL);
	controller->moveCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeMoveCallback, NULL);

	return controller;
}

ShovelerControllerTiltCallback *shovelerControllerAddTiltCallback(ShovelerController *controller, ShovelerControllerTiltCallbackFunction *callbackFunction, void *userData)
{
	ShovelerControllerTiltCallback *callback = malloc(sizeof(ShovelerControllerTiltCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(controller->tiltCallbacks, callback);
	return callback;
}

bool shovelerControllerRemoveTiltCallback(ShovelerController *controller, ShovelerControllerTiltCallback *tiltCallback)
{
	return g_hash_table_remove(controller->tiltCallbacks, tiltCallback);
}

ShovelerControllerMoveCallback *shovelerControllerAddMoveCallback(ShovelerController *controller, ShovelerControllerMoveCallbackFunction *callbackFunction, void *userData)
{
	ShovelerControllerMoveCallback *callback = malloc(sizeof(ShovelerControllerMoveCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(controller->moveCallbacks, callback);
	return callback;
}

bool shovelerControllerRemoveMoveCallback(ShovelerController *controller, ShovelerControllerMoveCallback *moveCallback)
{
	return g_hash_table_remove(controller->moveCallbacks, moveCallback);
}

void shovelerControllerUpdate(ShovelerController *controller, float dt)
{
	double newCursorX;
	double newCursorY;
	glfwGetCursorPos(controller->game->window, &newCursorX, &newCursorY);

	double cursorDiffX = newCursorX - controller->previousCursorX;
	double cursorDiffY = newCursorY - controller->previousCursorY;
	controller->previousCursorX = newCursorX;
	controller->previousCursorY = newCursorY;

	float tiltAmountX = controller->tiltFactor * (float) cursorDiffX;
	float tiltAmountY = controller->tiltFactor * (float) cursorDiffY;
	ShovelerVector2 tiltAmount = {tiltAmountX, tiltAmountY};
	triggerTilt(controller, tiltAmount);

	ShovelerVector3 moveAmount = {0.0f, 0.0f, 0.0f};
	int state;
	state = glfwGetKey(controller->game->window, GLFW_KEY_W);
	if(state == GLFW_PRESS) {
		moveAmount.values[2] += controller->moveFactor * dt;
	}

	state = glfwGetKey(controller->game->window, GLFW_KEY_S);
	if(state == GLFW_PRESS) {
		moveAmount.values[2] -= controller->moveFactor * dt;
	}

	state = glfwGetKey(controller->game->window, GLFW_KEY_A);
	if(state == GLFW_PRESS) {
		moveAmount.values[0] -= controller->moveFactor * dt;
	}

	state = glfwGetKey(controller->game->window, GLFW_KEY_D);
	if(state == GLFW_PRESS) {
		moveAmount.values[0] += controller->moveFactor * dt;
	}

	state = glfwGetKey(controller->game->window, GLFW_KEY_SPACE);
	if(state == GLFW_PRESS) {
		moveAmount.values[1] += controller->moveFactor * dt;
	}

	state = glfwGetKey(controller->game->window, GLFW_KEY_LEFT_CONTROL);
	if(state == GLFW_PRESS) {
		moveAmount.values[1] -= controller->moveFactor * dt;
	}

	triggerMove(controller, moveAmount);
}

void shovelerControllerFree(ShovelerController *controller)
{
	g_hash_table_destroy(controller->tiltCallbacks);
	g_hash_table_destroy(controller->moveCallbacks);
	free(controller);
}

static void triggerTilt(ShovelerController *controller, ShovelerVector2 tiltAmount)
{
	GHashTableIter iter;
	ShovelerControllerTiltCallback *callback;
	g_hash_table_iter_init(&iter, controller->tiltCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(controller, tiltAmount, callback->userData);
	}
}

static void triggerMove(ShovelerController *controller, ShovelerVector3 moveAmount)
{
	GHashTableIter iter;
	ShovelerControllerMoveCallback *callback;
	g_hash_table_iter_init(&iter, controller->moveCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(controller, moveAmount, callback->userData);
	}
}

static void freeTiltCallback(void *tiltCallbackPointer)
{
	free(tiltCallbackPointer);
}

static void freeMoveCallback(void *moveCallbackPointer)
{
	free(moveCallbackPointer);
}