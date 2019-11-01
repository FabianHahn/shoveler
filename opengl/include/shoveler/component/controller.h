#ifndef SHOVELER_COMPONENT_CONTROLLER_H
#define SHOVELER_COMPONENT_CONTROLLER_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerControllerStruct ShovelerController; // forward declaration: controller.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentViewTargetNameController = "controller";

ShovelerController *shovelerComponentGetViewController(ShovelerComponent *component);

static inline bool shovelerComponentHasViewController(ShovelerComponent *component)
{
	return shovelerComponentGetViewController(component) != NULL;
}

#endif
