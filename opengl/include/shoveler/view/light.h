#ifndef SHOVELER_VIEW_LIGHT_H
#define SHOVELER_VIEW_LIGHT_H

#include <glad/glad.h>

#include <shoveler/component/light.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h

typedef struct {
	/** Position dependency component to track. */
	long long int positionEntityId;
	ShovelerComponentLightType type;
	int width;
	int height;
	int samples;
	float ambientFactor;
	float exponentialFactor;
	ShovelerVector3 color;
} ShovelerViewLightConfiguration;

/** Adds a light component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration);
ShovelerLight *shovelerViewEntityGetLight(ShovelerViewEntity *entity);
/** Returns the current light configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetLightConfiguration(ShovelerViewEntity *entity, ShovelerViewLightConfiguration *outputConfiguration);
/** Updates a light component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration);
bool shovelerViewEntityRemoveLight(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetLightComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdLight);
}

#endif
