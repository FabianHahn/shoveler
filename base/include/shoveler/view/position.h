#ifndef SHOVELER_VIEW_POSITION_H
#define SHOVELER_VIEW_POSITION_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/view.h>

typedef void (ShovelerViewPositionRequestUpdateFunction)(ShovelerViewComponent *component, double x, double y, double z, void *userData);

typedef struct {
	double x;
	double y;
	double z;
	ShovelerViewPositionRequestUpdateFunction *requestUpdate;
	void *requestUpdateUserData;
} ShovelerViewPosition;

static const char *shovelerViewPositionComponentName = "position";

bool shovelerViewAddEntityPosition(ShovelerView *view, long long int entityId, double x, double y, double z);
bool shovelerViewUpdateEntityPosition(ShovelerView *view, long long int entityId, double x, double y, double z);
bool shovelerViewDelegatePosition(ShovelerView *view, long long int entityId, ShovelerViewPositionRequestUpdateFunction *requestUpdateFunction, void *userData);
bool shovelerViewUndelegatePosition(ShovelerView *view, long long int entityId);
bool shovelerViewRequestPositionUpdate(ShovelerView *view, long long int entityId, double x, double y, double z);
bool shovelerViewRemoveEntityPosition(ShovelerView *view, long long int entityId);

#endif
