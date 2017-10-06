#ifndef SHOVELER_CONTROLLER_H
#define SHOVELER_CONTROLLER_H

#include <shoveler/game.h>
#include <shoveler/types.h>

struct ShovelerControllerStruct; // forward declearation

typedef void (ShovelerControllerTiltCallback)(struct ShovelerControllerStruct *controller, ShovelerVector2 amount, void *userData);
typedef void (ShovelerControllerMoveCallback)(struct ShovelerControllerStruct *controller, ShovelerVector3 amount, void *userData);

typedef struct ShovelerControllerStruct {
	ShovelerGame *game;
	float moveFactor;
	float tiltFactor;
	double previousCursorX;
	double previousCursorY;
	ShovelerControllerTiltCallback *tilt;
	void *tiltUserData;
	ShovelerControllerMoveCallback *move;
	void *moveUserData;
} ShovelerController;

ShovelerController *shovelerControllerCreate(ShovelerGame *game, float moveFactor, float tiltFactor);
void shovelerControllerUpdate(ShovelerController *controller, float dt);
void shovelerControllerFree(ShovelerController *controller);

#endif