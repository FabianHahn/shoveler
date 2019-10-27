#ifndef SHOVELER_VIEW_RESOURCES_H
#define SHOVELER_VIEW_RESOURCES_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerResourcesStruct ShovelerResources; // forward declaration: resources.h
typedef struct ShovelerViewStruct ShovelerView; // forward declaration: view.h

static const char *shovelerViewResourcesTargetName = "resources";

bool shovelerViewSetResources(ShovelerView *view, ShovelerResources *resources);
ShovelerResources *shovelerViewGetResources(ShovelerView *view);
ShovelerResources *shovelerComponentGetViewResources(ShovelerComponent *component);

static inline bool shovelerViewHasResources(ShovelerView *view)
{
	return shovelerViewGetResources(view) != NULL;
}

static inline bool shovelerComponentHasViewResources(ShovelerComponent *component)
{
	return shovelerComponentGetViewResources(component) != NULL;
}


#endif
