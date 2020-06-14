#ifndef SHOVELER_COMPONENT_POSITION_H
#define SHOVELER_COMPONENT_POSITION_H

#include <shoveler/types.h>

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdPosition;

typedef enum {
	SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE,
	SHOVELER_COMPONENT_POSITION_TYPE_RELATIVE,
} ShovelerComponentPositionType;

typedef enum {
	SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE,
	SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES,
	SHOVELER_COMPONENT_POSITION_OPTION_ID_RELATIVE_PARENT_POSITION,
} ShovelerComponentPositionOptionId;

ShovelerComponentType *shovelerComponentCreatePositionType();
ShovelerVector3 shovelerComponentGetPositionCoordinates(ShovelerComponent *component);

#endif
