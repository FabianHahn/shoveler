#ifndef SHOVELER_VIEW_RESOURCES_H
#define SHOVELER_VIEW_RESOURCES_H

#include <stdbool.h> // bool
#include <stddef.h> // size_t

#include <shoveler/resources.h>
#include <shoveler/view.h>

typedef struct {
	char *typeId;
	char *resourceId;
} ShovelerViewResource;

static const char *shovelerViewResourceComponentName = "resource";
static const char *shovelerViewResourcesTargetName = "resources";

bool shovelerViewAddEntityResource(ShovelerView *view, long long int entityId, const char *typeId, const unsigned char *buffer, size_t bytes);
bool shovelerViewUpdateEntityResource(ShovelerView *view, long long int entityId, const unsigned char *buffer, size_t bytes);
bool shovelerViewRemoveEntityResource(ShovelerView *view, long long int entityId);

static inline ShovelerResources *shovelerViewGetResources(ShovelerView *view)
{
	return (ShovelerResources *) shovelerViewGetTarget(view, shovelerViewResourcesTargetName);
}

static inline bool shovelerViewHasResources(ShovelerView *view)
{
	return shovelerViewGetTarget(view, shovelerViewResourcesTargetName) != NULL;
}

#endif
