#ifndef SHOVELER_VIEW_TEXTURE_H
#define SHOVELER_VIEW_TEXTURE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/sampler.h>
#include <shoveler/texture.h>
#include <shoveler/view.h>

typedef struct {
	long long int imageResourceEntityId;
	bool interpolate;
	bool clamp;
} ShovelerViewTextureConfiguration;

static const char *shovelerViewTextureComponentName = "texture";

bool shovelerViewEntityAddTexture(ShovelerViewEntity *entity, ShovelerViewTextureConfiguration configuration);
ShovelerTexture *shovelerViewEntityGetTexture(ShovelerViewEntity *entity);
ShovelerSampler *shovelerViewEntityGetTextureSampler(ShovelerViewEntity *entity);
const ShovelerViewTextureConfiguration *shovelerViewEntityGetTextureConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateTexture(ShovelerViewEntity *entity, ShovelerViewTextureConfiguration configuration);
bool shovelerViewEntityRemoveTexture(ShovelerViewEntity *entity);

#endif
