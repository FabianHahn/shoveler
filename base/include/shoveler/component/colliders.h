#ifndef SHOVELER_COMPONENT_COLLIDERS_H
#define SHOVELER_COMPONENT_COLLIDERS_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerCollidersStruct ShovelerColliders; // forward declaration: colliders.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentViewTargetNameColliders = "colliders";

ShovelerColliders *shovelerComponentGetViewColliders(ShovelerComponent *component);

static inline bool shovelerComponentHasViewColliders(ShovelerComponent *component)
{
	return shovelerComponentGetViewColliders(component) != NULL;
}

#endif
