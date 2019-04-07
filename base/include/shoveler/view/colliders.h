#ifndef SHOVELER_VIEW_COLLIDERS_H
#define SHOVELER_VIEW_COLLIDERS_H

#include <stdbool.h> // bool
#include <stddef.h> // size_t

#include <shoveler/collider.h>
#include <shoveler/view.h>

struct ShovelerCollidersStruct; // forward declaration: colliders.h

static const char *shovelerViewCollidersTargetName = "colliders";

static inline bool shovelerViewSetColliders(ShovelerView *view, struct ShovelerCollidersStruct *colliders)
{
	return shovelerViewSetTarget(view, shovelerViewCollidersTargetName, colliders);
}

static inline struct ShovelerCollidersStruct *shovelerViewGetColliders(ShovelerView *view)
{
	return (struct ShovelerCollidersStruct *) shovelerViewGetTarget(view, shovelerViewCollidersTargetName);
}

static inline bool shovelerViewHasColliders(ShovelerView *view)
{
	return shovelerViewGetTarget(view, shovelerViewCollidersTargetName) != NULL;
}

#endif
