#ifndef SHOVELER_VIEW_DRAWABLE_H
#define SHOVELER_VIEW_DRAWABLE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/component/drawable.h>
#include <shoveler/drawable.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

typedef struct {
	ShovelerComponentDrawableType type;
	unsigned int tilesWidth;
	unsigned int tilesHeight;
} ShovelerViewDrawableConfiguration;

/** Adds a drawable component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddDrawable(ShovelerViewEntity *entity, const ShovelerViewDrawableConfiguration *configuration);
ShovelerDrawable *shovelerViewEntityGetDrawable(ShovelerViewEntity *entity);
/** Returns the current drawable configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetDrawableConfiguration(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration *outputConfiguration);
/** Updates a drawable component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateDrawable(ShovelerViewEntity *entity, const ShovelerViewDrawableConfiguration *configuration);
bool shovelerViewEntityRemoveDrawable(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetDrawableComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentTypeName);
}

#endif
