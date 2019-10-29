#ifndef SHOVELER_VIEW_CANVAS_H
#define SHOVELER_VIEW_CANVAS_H

#include <stdbool.h> // bool

#include <shoveler/component/canvas.h>
#include <shoveler/view.h>

typedef struct {
	size_t numTileSprites;
	const long long int *tileSpriteEntityIds;
} ShovelerViewCanvasConfiguration;

/** Adds a canvas component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration);
ShovelerCanvas *shovelerViewEntityGetCanvas(ShovelerViewEntity *entity);
/** Returns the current canvas configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetCanvasConfiguration(ShovelerViewEntity *entity, ShovelerViewCanvasConfiguration *outputConfiguration);
/** Updates a canvas component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration);
bool shovelerViewEntityRemoveCanvas(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetCanvasComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentTypeName);
}

#endif
