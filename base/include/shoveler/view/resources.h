#ifndef SHOVELER_VIEW_RESOURCES_H
#define SHOVELER_VIEW_RESOURCES_H

#include <stdbool.h> // bool
#include <stddef.h> // size_t

#include <shoveler/resources.h>
#include <shoveler/view.h>

typedef struct {
	const char *typeId;
	const unsigned char *buffer;
	int bufferSize;
} ShovelerViewResourceConfiguration;

static const char *shovelerViewResourceComponentName = "resource";
static const char *shovelerViewResourcesTargetName = "resources";

bool shovelerViewEntityAddResource(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration configuration);
void *shovelerViewEntityGetResource(ShovelerViewEntity *entity);
const ShovelerViewResourceConfiguration *shovelerViewEntityGetResourceConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateResource(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration configuration);
bool shovelerViewEntityRemoveResource(ShovelerViewEntity *entity);

static inline bool shovelerViewSetResources(ShovelerView *view, ShovelerResources *resources)
{
	return shovelerViewSetTarget(view, shovelerViewResourcesTargetName, resources);
}

static inline ShovelerResources *shovelerViewGetResources(ShovelerView *view)
{
	return (ShovelerResources *) shovelerViewGetTarget(view, shovelerViewResourcesTargetName);
}

static inline bool shovelerViewHasResources(ShovelerView *view)
{
	return shovelerViewGetTarget(view, shovelerViewResourcesTargetName) != NULL;
}

#endif
