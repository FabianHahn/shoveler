#ifndef SHOVELER_CONTROLLER_H
#define SHOVELER_CONTROLLER_H

#include <shoveler/game.h>
#include <shoveler/input.h>
#include <shoveler/types.h>

struct ShovelerControllerStruct; // forward declaration

typedef void (ShovelerControllerTiltCallbackFunction)(struct ShovelerControllerStruct *controller, ShovelerVector2 amount, void *userData);

typedef struct {
	ShovelerControllerTiltCallbackFunction *function;
	void *userData;
} ShovelerControllerTiltCallback;

typedef void (ShovelerControllerMoveCallbackFunction)(struct ShovelerControllerStruct *controller, ShovelerVector3 amount, void *userData);

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
	ShovelerGame *game;
	float moveFactor;
	float tiltFactor;
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

ShovelerController *shovelerControllerCreate(ShovelerGame *game, float moveFactor, float tiltFactor);
ShovelerControllerTiltCallback *shovelerControllerAddTiltCallback(ShovelerController *controller, ShovelerControllerTiltCallbackFunction *callbackFunction, void *userData);
bool shovelerControllerRemoveTiltCallback(ShovelerController *controller, ShovelerControllerTiltCallback *tiltCallback);
ShovelerControllerMoveCallback *shovelerControllerAddMoveCallback(ShovelerController *controller, ShovelerControllerMoveCallbackFunction *callbackFunction, void *userData);
bool shovelerControllerRemoveMoveCallback(ShovelerController *controller, ShovelerControllerMoveCallback *moveCallback);
ShovelerControllerAspectRatioChangeCallback *shovelerControllerAddAspectRatioChangeCallback(ShovelerController *controller, ShovelerControllerAspectRatioChangeCallbackFunction *callbackFunction, void *userData);
bool shovelerControllerRemoveAspectRatioChangeCallback(ShovelerController *controller, ShovelerControllerAspectRatioChangeCallback *aspectRatioChangeCallback);
void shovelerControllerUpdate(ShovelerController *controller, float dt);
void shovelerControllerFree(ShovelerController *controller);

#endif
