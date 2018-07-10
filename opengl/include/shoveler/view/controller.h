#ifndef SHOVELER_VIEW_CONTROLLER_H
#define SHOVELER_VIEW_CONTROLLER_H

#include <shoveler/controller.h>
#include <shoveler/view.h>

static inline ShovelerController *shovelerViewGetController(ShovelerView *view)
{
	return (ShovelerController *) shovelerViewGetTarget(view, "controller");
}

static inline bool shovelerViewHasController(ShovelerView *view)
{
	return shovelerViewGetController(view) != NULL;
}

#endif
