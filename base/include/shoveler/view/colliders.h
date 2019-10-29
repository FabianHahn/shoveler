#ifndef SHOVELER_VIEW_COLLIDERS_H
#define SHOVELER_VIEW_COLLIDERS_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerCollidersStruct ShovelerColliders; // forward declaration: colliders.h
typedef struct ShovelerViewStruct ShovelerView; // forward declaration: view.h

bool shovelerViewSetColliders(ShovelerView *view, ShovelerColliders *colliders);
ShovelerColliders *shovelerViewGetColliders(ShovelerView *view);

static inline bool shovelerViewHasColliders(ShovelerView *view)
{
	return shovelerViewGetColliders(view) != NULL;
}

#endif
