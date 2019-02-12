#ifndef SHOVELER_VIEW_COLLIDERS_H
#define SHOVELER_VIEW_COLLIDERS_H

#include <stdbool.h> // bool
#include <stddef.h> // size_t

#include <shoveler/collider.h>
#include <shoveler/view.h>

static const char *shovelerViewCollidersTargetName = "colliders";

static inline bool shovelerViewSetColliders(ShovelerView *view, ShovelerColliders *colliders)
{
	return shovelerViewSetTarget(view, shovelerViewCollidersTargetName, colliders);
}

static inline ShovelerColliders *shovelerViewGetColliders(ShovelerView *view)
{
	return (ShovelerColliders *) shovelerViewGetTarget(view, shovelerViewCollidersTargetName);
}

static inline bool shovelerViewHasColliders(ShovelerView *view)
{
	return shovelerViewGetTarget(view, shovelerViewCollidersTargetName) != NULL;
}

#endif
