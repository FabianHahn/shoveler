#ifndef SHOVELER_VIEW_POSITION_H
#define SHOVELER_VIEW_POSITION_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/view.h>

typedef void (ShovelerViewPositionRequestUpdateFunction)(ShovelerViewComponent *component, double x, double y, double z, void *userData);

static const char *shovelerViewPositionComponentName = "position";

typedef struct {
	double x;
	double y;
	double z;
} ShovelerViewPosition;

bool shovelerViewEntityAddPosition(ShovelerViewEntity *entity, double x, double y, double z);
ShovelerViewPosition *shovelerViewEntityGetPosition(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdatePosition(ShovelerViewEntity *entity, double x, double y, double z);
bool shovelerViewEntityDelegatePosition(ShovelerViewEntity *entity, ShovelerViewPositionRequestUpdateFunction *requestUpdateFunction, void *userData);
bool shovelerViewEntityUndelegatePosition(ShovelerViewEntity *entity);
bool shovelerViewEntityRequestPositionUpdate(ShovelerViewEntity *entity, double x, double y, double z);
bool shovelerViewEntityRemoveEntityPosition(ShovelerViewEntity *entity);

#endif
