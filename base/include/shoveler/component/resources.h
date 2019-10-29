#ifndef SHOVELER_COMPONENT_RESOURCES_H
#define SHOVELER_COMPONENT_RESOURCES_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerResourcesStruct ShovelerResources; // forward declaration: resources.h

static const char *shovelerComponentViewTargetNameResources = "resources";

ShovelerResources *shovelerComponentGetViewResources(ShovelerComponent *component);

static inline bool shovelerComponentHasViewResources(ShovelerComponent *component)
{
	return shovelerComponentGetViewResources(component) != NULL;
}

#endif
