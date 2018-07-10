#ifndef SHOVELER_VIEW_SCENE_H
#define SHOVELER_VIEW_SCENE_H

#include <shoveler/scene.h>
#include <shoveler/view.h>

static inline ShovelerScene *shovelerViewGetScene(ShovelerView *view)
{
	return (ShovelerScene *) shovelerViewGetTarget(view, "scene");
}

static inline bool shovelerViewHasScene(ShovelerView *view)
{
	return shovelerViewGetScene(view) != NULL;
}

#endif
