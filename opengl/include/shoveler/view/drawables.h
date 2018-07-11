#ifndef SHOVELER_VIEW_DRAWABLES_H
#define SHOVELER_VIEW_DRAWABLES_H

#include <shoveler/drawable.h>
#include <shoveler/view.h>

typedef struct {
	ShovelerDrawable *cube;
	ShovelerDrawable *quad;
	ShovelerDrawable *point;
} ShovelerViewDrawables;

static const char *shovelerViewDrawablesTargetName = "drawables";

ShovelerViewDrawables *shovelerViewDrawablesCreate(ShovelerView *view);
void shovelerViewDrawablesFree(ShovelerViewDrawables *drawables);

static inline ShovelerViewDrawables *shovelerViewGetDrawables(ShovelerView *view)
{
	return (ShovelerViewDrawables *) shovelerViewGetTarget(view, shovelerViewDrawablesTargetName);
}

static inline bool shovelerViewHasDrawables(ShovelerView *view)
{
	return shovelerViewGetDrawables(view) != NULL;
}

#endif
