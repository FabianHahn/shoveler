#ifndef SHOVELER_CONTROLLER_H
#define SHOVELER_CONTROLLER_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/input.h>
#include <shoveler/types.h>

struct ShovelerCollidersStruct; // forward declaration: colliders.h
struct ShovelerControllerStruct; // forward declaration: below

typedef void (ShovelerControllerTiltCallbackFunction)(struct ShovelerControllerStruct *controller, ShovelerVector3 direction, ShovelerVector3 upwards, void *userData);

typedef struct {
	ShovelerControllerTiltCallbackFunction *function;
	void *userData;
} ShovelerControllerTiltCallback;

typedef void (ShovelerControllerMoveCallbackFunction)(struct ShovelerControllerStruct *controller, ShovelerVector3 position, void *userData);

typedef struct {
	ShovelerControllerMoveCallbackFunction *function;
	void *userData;
} ShovelerControllerMoveCallback;

typedef void (ShovelerControllerAspectRatioChangeCallbackFunction)(struct ShovelerControllerStruct *controller, float aspectRatio, void *userData);

typedef struct {
	ShovelerControllerAspectRatioChangeCallbackFunction *function;
	void *userData;
} ShovelerControllerAspectRatioChangeCallback;

typedef struct ShovelerControllerStruct {
	GLFWwindow *window;
	ShovelerInput *input;
	struct ShovelerCollidersStruct *colliders;
	/** normalized current reference frame of the controller */
	ShovelerReferenceFrame frame;
	/** static "up direction vector", which is potentially different from the current frame's up direction */
	ShovelerVector3 up;
	float moveFactor;
	float tiltFactor;
	float boundingBoxSize2;
	float boundingBoxSize3;
	bool lockMoveX;
	bool lockMoveY;
	bool lockMoveZ;
	bool lockTiltX;
	bool lockTiltY;
	double previousCursorX;
	double previousCursorY;
	/** set of (ShovelerControllerTiltCallback *) */
	GHashTable *tiltCallbacks;
	/** set of (ShovelerControllerMoveCallback *) */
	GHashTable *moveCallbacks;
	/** set of (ShovelerControllerAspectRatioChangeCallback *) */
	GHashTable *aspectRatioChangeCallbacks;
	ShovelerInputWindowSizeCallback *windowSizeCallback;
} ShovelerController;

/** Create a controller using a window and an input system from an initial reference frame that is copied. */
ShovelerController *shovelerControllerCreate(GLFWwindow *window, ShovelerInput *input, struct ShovelerCollidersStruct *colliders, const ShovelerReferenceFrame *frame, float moveFactor, float tiltFactor, float boundingBoxSize2, float boundingBoxSize3);
ShovelerControllerTiltCallback *shovelerControllerAddTiltCallback(ShovelerController *controller, ShovelerControllerTiltCallbackFunction *callbackFunction, void *userData);
bool shovelerControllerRemoveTiltCallback(ShovelerController *controller, ShovelerControllerTiltCallback *tiltCallback);
ShovelerControllerMoveCallback *shovelerControllerAddMoveCallback(ShovelerController *controller, ShovelerControllerMoveCallbackFunction *callbackFunction, void *userData);
bool shovelerControllerRemoveMoveCallback(ShovelerController *controller, ShovelerControllerMoveCallback *moveCallback);
ShovelerControllerAspectRatioChangeCallback *shovelerControllerAddAspectRatioChangeCallback(ShovelerController *controller, ShovelerControllerAspectRatioChangeCallbackFunction *callbackFunction, void *userData);
bool shovelerControllerRemoveAspectRatioChangeCallback(ShovelerController *controller, ShovelerControllerAspectRatioChangeCallback *aspectRatioChangeCallback);
void shovelerControllerUpdate(ShovelerController *controller, float dt);
void shovelerControllerFree(ShovelerController *controller);

#endif
