#include <stdlib.h> // malloc, free

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shoveler/controller.h"

ShovelerController *shovelerControllerCreate(ShovelerGame *game, float moveFactor, float tiltFactor)
{
	ShovelerController *controller = malloc(sizeof(ShovelerController));
	controller->game = game;
	controller->moveFactor = moveFactor;
	controller->tiltFactor = tiltFactor;

	glfwGetCursorPos(game->window, &controller->previousCursorX, &controller->previousCursorY);

	controller->tilt = NULL;
	controller->tiltUserData = NULL;
	controller->move = NULL;
	controller->moveUserData = NULL;

	return controller;
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
	if(controller->tilt != NULL) {
		controller->tilt(controller, tiltAmount, controller->tiltUserData);
	}

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

	if(controller->move != NULL) {
		controller->move(controller, moveAmount, controller->moveUserData);
	}
}

void shovelerControllerFree(ShovelerController *controller)
{
	free(controller);
}
