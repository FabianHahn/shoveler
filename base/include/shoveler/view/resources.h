#ifndef SHOVELER_VIEW_RESOURCES_H
#define SHOVELER_VIEW_RESOURCES_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerResourcesStruct ShovelerResources; // forward declaration: resources.h
typedef struct ShovelerViewStruct ShovelerView; // forward declaration: view.h

bool shovelerViewSetResources(ShovelerView *view, ShovelerResources *resources);
ShovelerResources *shovelerViewGetResources(ShovelerView *view);

static inline bool shovelerViewHasResources(ShovelerView *view)
{
	return shovelerViewGetResources(view) != NULL;
}

#endif
