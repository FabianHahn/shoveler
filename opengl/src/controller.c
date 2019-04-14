#include <stdlib.h> // malloc, free

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shoveler/collider.h"
#include "shoveler/colliders.h"
#include "shoveler/controller.h"
#include "shoveler/log.h"
#include "shoveler/input.h"
#include "shoveler/types.h"

static void updatePosition(ShovelerController *controller, float dt);
static void updateTilt(ShovelerController *controller, float dt);
static void shiftOrientation(ShovelerController *controller, ShovelerVector2 tiltAmount);
static void shiftPosition(ShovelerController *controller, ShovelerVector3 moveAmount);
static void windowSizeHandler(ShovelerInput *input, int width, int height, void *controllerPointer);
static void triggerTilt(ShovelerController *controller, ShovelerVector3 direction, ShovelerVector3 orientation);
static void triggerMove(ShovelerController *controller, ShovelerVector3 moveAmount);
static void triggerAspectRatioChange(ShovelerController *controller, float aspectRatio);
static void freeTiltCallback(void *tiltCallbackPointer);
static void freeMoveCallback(void *moveCallbackPointer);
static void freeAspectRatioChangeCallback(void *aspectRatioCallbackChangePointer);

ShovelerController *shovelerControllerCreate(GLFWwindow *window, ShovelerInput *input, ShovelerColliders *colliders, const ShovelerReferenceFrame *frame, float moveFactor, float tiltFactor, float boundingBoxSize2, float boundingBoxSize3)
{
	ShovelerController *controller = malloc(sizeof(ShovelerController));
	controller->window = window;
	controller->input = input;
	controller->colliders = colliders;

	controller->frame.position = frame->position;
	controller->frame.direction = shovelerVector3Normalize(frame->direction);
	controller->up = shovelerVector3Normalize(frame->up);

	ShovelerVector3 right = shovelerVector3Normalize(shovelerVector3Cross(controller->frame.direction, controller->up));
	controller->frame.up = shovelerVector3Cross(right, controller->frame.direction);

	controller->moveFactor = moveFactor;
	controller->tiltFactor = tiltFactor;

	controller->boundingBoxSize2 = boundingBoxSize2;
	controller->boundingBoxSize3 = boundingBoxSize3;

	controller->lockMoveX = false;
	controller->lockMoveY = false;
	controller->lockMoveZ = false;
	controller->lockTiltX = false;
	controller->lockTiltY = false;

	glfwGetCursorPos(controller->window, &controller->previousCursorX, &controller->previousCursorY);

	controller->tiltCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeTiltCallback, NULL);
	controller->moveCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeMoveCallback, NULL);
	controller->aspectRatioChangeCallbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeAspectRatioChangeCallback, NULL);

	controller->windowSizeCallback = shovelerInputAddWindowSizeCallback(controller->input, windowSizeHandler, controller);

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

ShovelerControllerAspectRatioChangeCallback *shovelerControllerAddAspectRatioChangeCallback(ShovelerController *controller, ShovelerControllerAspectRatioChangeCallbackFunction *callbackFunction, void *userData)
{
	ShovelerControllerAspectRatioChangeCallback *callback = malloc(sizeof(ShovelerControllerAspectRatioChangeCallback));
	callback->function = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(controller->aspectRatioChangeCallbacks, callback);
	return callback;
}

bool shovelerControllerRemoveAspectRatioChangeCallback(ShovelerController *controller, ShovelerControllerAspectRatioChangeCallback *aspectRatioChangeCallback)
{
	return g_hash_table_remove(controller->aspectRatioChangeCallbacks, aspectRatioChangeCallback);
}

void shovelerControllerUpdate(ShovelerController *controller, float dt)
{
	if(glfwGetInputMode(controller->window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		return;
	}

	updatePosition(controller, dt);
	updateTilt(controller, dt);
}

void shovelerControllerFree(ShovelerController *controller)
{
	shovelerInputRemoveWindowSizeCallback(controller->input, controller->windowSizeCallback);

	g_hash_table_destroy(controller->tiltCallbacks);
	g_hash_table_destroy(controller->moveCallbacks);
	g_hash_table_destroy(controller->aspectRatioChangeCallbacks);
	free(controller);
}

static void updatePosition(ShovelerController *controller, float dt)
{
	ShovelerVector3 moveAmount = {0.0f, 0.0f, 0.0f};
	if(!controller->lockMoveX) {
		if (glfwGetKey(controller->window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(controller->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			moveAmount.values[0] -= controller->moveFactor * dt;
		}

		if (glfwGetKey(controller->window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(controller->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			moveAmount.values[0] += controller->moveFactor * dt;
		}
	}

	if(!controller->lockMoveY) {
		if (glfwGetKey(controller->window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(controller->window, GLFW_KEY_UP) == GLFW_PRESS) {
			moveAmount.values[1] += controller->moveFactor * dt;
		}

		if (glfwGetKey(controller->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(controller->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			moveAmount.values[1] -= controller->moveFactor * dt;
		}
	}

	if(!controller->lockMoveZ) {
		if (glfwGetKey(controller->window, GLFW_KEY_W) == GLFW_PRESS) {
			moveAmount.values[2] += controller->moveFactor * dt;
		}

		if (glfwGetKey(controller->window, GLFW_KEY_S) == GLFW_PRESS) {
			moveAmount.values[2] -= controller->moveFactor * dt;
		}
	}

	if(moveAmount.values[0] != 0.0f || moveAmount.values[1] != 0.0f || moveAmount.values[2] != 0.0f) {
		shiftPosition(controller, moveAmount);
	}
}

static void updateTilt(ShovelerController *controller, float dt)
{
	double newCursorX;
	double newCursorY;
	glfwGetCursorPos(controller->window, &newCursorX, &newCursorY);

	double cursorDiffX = newCursorX - controller->previousCursorX;
	double cursorDiffY = newCursorY - controller->previousCursorY;
	controller->previousCursorX = newCursorX;
	controller->previousCursorY = newCursorY;

	float tiltAmountX = controller->tiltFactor * (float) cursorDiffX;
	float tiltAmountY = controller->tiltFactor * (float) cursorDiffY;
	ShovelerVector2 tiltAmount = {
			controller->lockTiltX ? 0.0f : tiltAmountX,
			controller->lockTiltY ? 0.0f : tiltAmountY,
	};

	if(tiltAmount.values[0] != 0.0f || tiltAmount.values[1] != 0.0f) {
		shiftOrientation(controller, tiltAmount);
	}
}

static void shiftPosition(ShovelerController *controller, ShovelerVector3 moveAmount)
{
	ShovelerVector3 right = shovelerVector3Cross(controller->frame.direction, controller->frame.up);

	ShovelerVector3 targetPosition;
	targetPosition = shovelerVector3LinearCombination(1.0, controller->frame.position, moveAmount.values[2], controller->frame.direction);
	targetPosition = shovelerVector3LinearCombination(1.0, targetPosition, moveAmount.values[0], right);
	targetPosition = shovelerVector3LinearCombination(1.0, targetPosition, moveAmount.values[1], controller->frame.up);

	// check for 2d collisions
	if(controller->boundingBoxSize2 > 0.0f) {
		ShovelerVector2 targetPosition2 = shovelerVector2(targetPosition.values[0], targetPosition.values[1]);

		ShovelerBoundingBox2 targetBoundingBox2 = shovelerBoundingBox2(
				shovelerVector2LinearCombination(1.0, targetPosition2, -0.5f * controller->boundingBoxSize2, shovelerVector2(1.0f, 1.0f)),
				shovelerVector2LinearCombination(1.0, targetPosition2, 0.5f * controller->boundingBoxSize2, shovelerVector2(1.0f, 1.0f)));

		ShovelerCollider2 *collider = shovelerCollidersIntersect2(controller->colliders, &targetBoundingBox2);
		if(collider != NULL) {
			shovelerLogTrace("Bumping into 2d collider %p at (%.2f, %.2f), aborting position shift.", collider, targetPosition2.values[0], targetPosition2.values[1]);
			return;
		}
	}

	// check for 3d collisions
	if(controller->boundingBoxSize3 > 0.0f) {
		ShovelerBoundingBox3 targetBoundingBox3 = shovelerBoundingBox3(
				shovelerVector3LinearCombination(1.0, targetPosition, -0.5f * controller->boundingBoxSize3, shovelerVector3(1.0f, 1.0f, 1.0f)),
				shovelerVector3LinearCombination(1.0, targetPosition, 0.5f * controller->boundingBoxSize3, shovelerVector3(1.0f, 1.0f, 1.0f)));

		ShovelerCollider3 *collider = shovelerCollidersIntersect3(controller->colliders, &targetBoundingBox3);
		if(collider != NULL) {
			shovelerLogTrace("Bumping into 3d collider %p at (%.2f, %.2f, %.2f), aborting position shift.", collider, targetPosition.values[0], targetPosition.values[1], targetPosition.values[2]);
			return;
		}
	}

	controller->frame.position = targetPosition;
	triggerMove(controller, controller->frame.position);
}

static void shiftOrientation(ShovelerController *controller, ShovelerVector2 tiltAmount)
{
	// Rotate camera in x direction
	ShovelerMatrix rotationX = shovelerMatrixCreateRotation(controller->up, tiltAmount.values[0]);
	ShovelerMatrix normalRotationX = shovelerMatrixTranspose(rotationX);

	controller->frame.direction = shovelerVector3Normalize(shovelerMatrixMultiplyVector3(normalRotationX, controller->frame.direction));

	ShovelerVector3 right = shovelerVector3Normalize(shovelerVector3Cross(controller->frame.direction, controller->up));
	controller->frame.up = shovelerVector3Cross(right, controller->frame.direction);

	// Rotate camera in y direction
	ShovelerMatrix rotationY = shovelerMatrixCreateRotation(right, tiltAmount.values[1]);
	ShovelerMatrix normalRotationY = shovelerMatrixTranspose(rotationY);

	ShovelerVector3 newDirection = shovelerVector3Normalize(shovelerMatrixMultiplyVector3(normalRotationY, controller->frame.direction));
	ShovelerVector3 newFrameUp = shovelerVector3Cross(right, newDirection);

	if(shovelerVector3Dot(controller->up, newFrameUp) > 0.0f) { // only update if we're not flipping upside down
		controller->frame.direction = newDirection;
		controller->frame.up = newFrameUp;
	}

	triggerTilt(controller, controller->frame.direction, controller->frame.up);
}

static void windowSizeHandler(ShovelerInput *input, int width, int height, void *controllerPointer)
{
	ShovelerController *controller = controllerPointer;
	triggerAspectRatioChange(controller, (float) width / height);
}

static void triggerTilt(ShovelerController *controller, ShovelerVector3 direction, ShovelerVector3 upwards)
{
	GHashTableIter iter;
	ShovelerControllerTiltCallback *callback;
	g_hash_table_iter_init(&iter, controller->tiltCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(controller, direction, upwards, callback->userData);
	}
}

static void triggerMove(ShovelerController *controller, ShovelerVector3 position)
{
	GHashTableIter iter;
	ShovelerControllerMoveCallback *callback;
	g_hash_table_iter_init(&iter, controller->moveCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(controller, position, callback->userData);
	}
}

static void triggerAspectRatioChange(ShovelerController *controller, float aspectRatio)
{
	GHashTableIter iter;
	ShovelerControllerAspectRatioChangeCallback *callback;
	g_hash_table_iter_init(&iter, controller->aspectRatioChangeCallbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		callback->function(controller, aspectRatio, callback->userData);
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

static void freeAspectRatioChangeCallback(void *aspectRatioCallbackChangePointer)
{
	free(aspectRatioCallbackChangePointer);
}
