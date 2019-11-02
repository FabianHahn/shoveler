#ifndef SHOVELER_COMPONENT_POSITION_H
#define SHOVELER_COMPONENT_POSITION_H

#include <shoveler/types.h>

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdPosition;

typedef enum {
	SHOVELER_COMPONENT_POSITION_OPTION_COORDINATES,
} ShovelerComponentPositionOptionId;

ShovelerComponentType *shovelerComponentCreatePositionType();
const ShovelerVector3 *shovelerComponentGetPositionCoordinates(ShovelerComponent *component);

#endif
