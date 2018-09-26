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

bool shovelerViewAddEntityDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration configuration);
ShovelerDrawable *shovelerViewGetEntityDrawable(ShovelerView *view, long long int entityId);
ShovelerDrawable *shovelerViewEntityGetDrawable(ShovelerViewEntity *entity);
bool shovelerViewUpdateEntityDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration configuration);
bool shovelerViewRemoveEntityDrawable(ShovelerView *view, long long int entityId);

#endif
