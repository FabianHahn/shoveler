#ifndef SHOVELER_COMPONENT_POSITION_H
#define SHOVELER_COMPONENT_POSITION_H

#include <shoveler/types.h>

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNamePosition = "position";
static const char *shovelerComponentPositionOptionKeyCoordinates = "coordinates";

ShovelerComponentType *shovelerComponentCreatePositionType();
const ShovelerVector3 *shovelerComponentGetPositionCoordinates(ShovelerComponent *component);

#endif
