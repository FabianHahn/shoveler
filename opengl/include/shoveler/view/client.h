#ifndef SHOVELER_VIEW_CLIENT_H
#define SHOVELER_VIEW_CLIENT_H

#include <stdbool.h>

#include <shoveler/view.h>

typedef struct {
	/** Position dependency component to track. */
	long long int positionEntityId;
	/** Optional: If nonzero, depends on the model component and disables its visibility. */
	long long int modelEntityId;
} ShovelerViewClientConfiguration;

static const char *shovelerViewClientComponentTypeName = "client";
static const char *shovelerViewClientPositionOptionKey = "position";
static const char *shovelerViewClientModelOptionKey = "model";

/** Adds a client component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration);
bool shovelerViewEntityGetClientConfiguration(ShovelerViewEntity *entity, ShovelerViewClientConfiguration *outputConfiguration);
/** Updates a client component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration);
bool shovelerViewEntityRemoveClient(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetClientComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewClientComponentTypeName);
}

#endif
