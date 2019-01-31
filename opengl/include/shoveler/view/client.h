#ifndef SHOVELER_VIEW_CLIENT_H
#define SHOVELER_VIEW_CLIENT_H

#include <stdbool.h>

#include <shoveler/view.h>

static const char *shovelerViewClientComponentName = "client";

typedef struct {
	/** Position dependency component to track. */
	long long int positionEntityId;
	/** Optional: If nonzero, depends on the model component. */
	long long int modelEntityId;
	/** If there is a model dependency, specifies whether its visibility should be disabled on gaining authority. */
	bool disableModelVisibility;
} ShovelerViewClientConfiguration;

/** Adds a client component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration);
const ShovelerViewClientConfiguration *shovelerViewEntityGetClientConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityDelegateClient(ShovelerViewEntity *entity);
bool shovelerViewEntityUndelegateClient(ShovelerViewEntity *entity);
/** Updates a client component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration);
bool shovelerViewEntityRemoveClient(ShovelerViewEntity *entity);

#endif
