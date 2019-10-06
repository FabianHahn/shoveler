#ifndef SHOVELER_VIEW_TEXTURE_H
#define SHOVELER_VIEW_TEXTURE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/texture.h>
#include <shoveler/view.h>

typedef struct {
	long long int imageResourceEntityId;
} ShovelerViewTextureConfiguration;

static const char *shovelerViewTextureComponentTypeName = "texture";
static const char *shovelerViewTextureImageResourceOptionKey = "image_resource";

/** Adds a texture component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddTexture(ShovelerViewEntity *entity, const ShovelerViewTextureConfiguration *configuration);
ShovelerTexture *shovelerViewEntityGetTexture(ShovelerViewEntity *entity);
/** Returns the current texture configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetTextureConfiguration(ShovelerViewEntity *entity, ShovelerViewTextureConfiguration *outputConfiguration);
/** Updates a texture component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTexture(ShovelerViewEntity *entity, const ShovelerViewTextureConfiguration *configuration);
bool shovelerViewEntityRemoveTexture(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetTextureComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentTypeName);
}

#endif
