#ifndef SHOVELER_VIEW_CONTROLLER_H
#define SHOVELER_VIEW_CONTROLLER_H

#include <shoveler/controller.h>
#include <shoveler/view.h>

bool shovelerViewSetController(ShovelerView *view, ShovelerController *controller);
ShovelerController *shovelerViewGetController(ShovelerView *view);

static inline bool shovelerViewHasController(ShovelerView *view)
{
	return shovelerViewGetController(view) != NULL;
}

#endif
