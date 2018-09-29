#ifndef SHOVELER_VIEW_DRAWABLE_H
#define SHOVELER_VIEW_DRAWABLE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/drawable.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

typedef enum {
	SHOVELER_VIEW_DRAWABLE_TYPE_CUBE,
	SHOVELER_VIEW_DRAWABLE_TYPE_QUAD,
	SHOVELER_VIEW_DRAWABLE_TYPE_POINT,
	SHOVELER_VIEW_DRAWABLE_TYPE_TILES,
} ShovelerViewDrawableType;

typedef struct {
	unsigned char width;
	unsigned char height;
} ShovelerViewDrawableTilesSize;

typedef struct {
	ShovelerViewDrawableType type;
	ShovelerViewDrawableTilesSize tilesSize;
} ShovelerViewDrawableConfiguration;

static const char *shovelerViewDrawableComponentName = "drawable";

bool shovelerViewEntityAddDrawable(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration configuration);
ShovelerDrawable *shovelerViewEntityGetDrawable(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateDrawable(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration configuration);
bool shovelerViewEntityRemoveDrawable(ShovelerViewEntity *entity);

#endif
