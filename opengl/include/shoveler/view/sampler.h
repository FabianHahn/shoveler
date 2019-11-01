#ifndef SHOVELER_VIEW_SAMPLER_H
#define SHOVELER_VIEW_SAMPLER_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/component/sampler.h>
#include <shoveler/sampler.h>
#include <shoveler/view.h>

typedef struct {
	bool interpolate;
	bool useMipmaps;
	bool clamp;
} ShovelerViewSamplerConfiguration;

/** Adds a sampler component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration);
ShovelerSampler *shovelerViewEntityGetSampler(ShovelerViewEntity *entity);
/** Returns the current sampler configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetSamplerConfiguration(ShovelerViewEntity *entity, ShovelerViewSamplerConfiguration *outputConfiguration);
/** Updates a sampler component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateSampler(ShovelerViewEntity *entity, const ShovelerViewSamplerConfiguration *configuration);
bool shovelerViewEntityRemoveSampler(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetSamplerComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewSamplerComponentTypeName);
}

#endif
