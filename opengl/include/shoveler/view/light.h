#ifndef SHOVELER_VIEW_LIGHT_H
#define SHOVELER_VIEW_LIGHT_H

#include <glad/glad.h>

#include <shoveler/types.h>
#include <shoveler/view.h>

typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h

typedef enum {
	SHOVELER_VIEW_LIGHT_TYPE_SPOT,
	SHOVELER_VIEW_LIGHT_TYPE_POINT
} ShovelerViewLightType;

typedef struct {
	/** Position dependency component to track. */
	long long int positionEntityId;
	ShovelerViewLightType type;
	int width;
	int height;
	int samples;
	float ambientFactor;
	float exponentialFactor;
	ShovelerVector3 color;
} ShovelerViewLightConfiguration;

static const char *shovelerViewLightComponentTypeName = "light";
static const char *shovelerViewLightPositionOptionKey = "position";
static const char *shovelerViewLightTypeOptionKey = "type";
static const char *shovelerViewLightWidthOptionKey = "width";
static const char *shovelerViewLightHeightOptionKey = "height";
static const char *shovelerViewLightSamplesOptionKey = "samples";
static const char *shovelerViewLightAmbientFactorOptionKey = "ambientFactor";
static const char *shovelerViewLightExponentialFactorOptionKey = "exponentialFactor";
static const char *shovelerViewLightColorOptionKey = "color";

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
	return shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
}

#endif
