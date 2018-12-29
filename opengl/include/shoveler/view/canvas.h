#ifndef SHOVELER_VIEW_CANVAS_H
#define SHOVELER_VIEW_CANVAS_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/material/canvas.h>
#include <shoveler/canvas.h>
#include <shoveler/tileset.h>
#include <shoveler/view.h>

typedef struct {
	int numTileSprites;
	long long int *tileSpriteEntityIds;
} ShovelerViewCanvasConfiguration;

static const char *shovelerViewCanvasComponentName = "canvas";

/** Adds a canvas component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration);
ShovelerCanvas *shovelerViewEntityGetCanvas(ShovelerViewEntity *entity);
const ShovelerViewCanvasConfiguration *shovelerViewEntityGetCanvasConfiguration(ShovelerViewEntity *entity);
/** Updates a canvas component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateCanvas(ShovelerViewEntity *entity, ShovelerViewCanvasConfiguration configuration);
bool shovelerViewEntityRemoveCanvas(ShovelerViewEntity *entity);

#endif
