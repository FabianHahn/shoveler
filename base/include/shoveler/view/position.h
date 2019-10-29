#ifndef SHOVELER_VIEW_POSITION_H
#define SHOVELER_VIEW_POSITION_H

#include <stdbool.h> // bool

#include <shoveler/component/position.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

ShovelerComponent *shovelerViewEntityAddPosition(ShovelerViewEntity *entity, ShovelerVector3 coordinates);
const ShovelerVector3 *shovelerViewEntityGetPositionCoordinates(ShovelerViewEntity *entity);
bool shovelerViewEntityRemovePosition(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetPositionComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeNamePosition);
}

#endif
