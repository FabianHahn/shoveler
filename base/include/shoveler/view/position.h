#ifndef SHOVELER_VIEW_POSITION_H
#define SHOVELER_VIEW_POSITION_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/view.h>
#include <shoveler/types.h>

typedef void (ShovelerViewPositionRequestUpdateFunction)(ShovelerComponent *component, ShovelerVector3 coordinates, void *userData);

static const char *shovelerViewPositionComponentTypeName = "position";
static const char *shovelerViewPositionCoordinatesOptionKey = "coordinates";

ShovelerComponent *shovelerViewEntityAddPosition(ShovelerViewEntity *entity, ShovelerVector3 coordinates);
const ShovelerVector3 *shovelerViewEntityGetPositionCoordinates(ShovelerViewEntity *entity);
bool shovelerViewEntityRemovePosition(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetPositionComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentTypeName);
}

#endif
