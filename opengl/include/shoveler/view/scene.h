#ifndef SHOVELER_VIEW_SCENE_H
#define SHOVELER_VIEW_SCENE_H

#include <shoveler/scene.h>
#include <shoveler/view.h>

bool shovelerViewSetScene(ShovelerView *view, ShovelerScene *scene);
ShovelerScene *shovelerViewGetScene(ShovelerView *view);

static inline bool shovelerViewHasScene(ShovelerView *view)
{
	return shovelerViewGetScene(view) != NULL;
}

#endif
