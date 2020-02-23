#ifndef SHOVELER_VIEW_RESOURCE_H
#define SHOVELER_VIEW_RESOURCE_H

#include <stdbool.h> // bool
#include <stddef.h> // size_t

#include <shoveler/component/resource.h>
#include <shoveler/view.h>

typedef struct {
	const unsigned char *buffer;
	int bufferSize;
} ShovelerViewResourceConfiguration;

ShovelerComponent *shovelerViewEntityAddResource(ShovelerViewEntity *entity, const ShovelerViewResourceConfiguration *configuration);
bool shovelerViewEntityRemoveResource(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetResourceComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdResource);
}

#endif
