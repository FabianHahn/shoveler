#ifndef SHOVELER_VIEW_SAMPLER_H
#define SHOVELER_VIEW_SAMPLER_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/sampler.h>
#include <shoveler/view.h>

typedef struct {
	bool interpolate;
	bool useMipmaps;
	bool clamp;
} ShovelerViewSamplerConfiguration;

static const char *shovelerViewSamplerComponentTypeName = "sampler";
static const char *shovelerViewSamplerInterpolateOptionKey = "interpolate";
static const char *shovelerViewSamplerUseMipmapsOptionKey = "use_mipmaps";
static const char *shovelerViewSamplerClampOptionKey = "clamp";

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
