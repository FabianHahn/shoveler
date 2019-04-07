#ifndef SHOVELER_COLLIDERS_H
#define SHOVELER_COLLIDERS_H

#include <glib.h>

#include <shoveler/types.h>

struct ShovelerCollider2Struct; // forward declaration: collider.h
struct ShovelerCollider3Struct; // forward declaration: collider.h

typedef struct ShovelerCollidersStruct {
	/** Hash set of (ShovelerCollider2 *) */
	/* private */ GHashTable *colliders2;
	/** Hash set of (ShovelerCollider3 *) */
	/* private */ GHashTable *colliders3;
} ShovelerColliders;

ShovelerColliders *shovelerCollidersCreate();
/** Adds a 2d collider to the colliders, with the caller retaining ownership over it and changes to it being reflected live. */
bool shovelerCollidersAddCollider2(ShovelerColliders *colliders, struct ShovelerCollider2Struct *collider);
/** Adds a 3d collider to the colliders, with the caller retaining ownership over it and changes to it being reflected live. */
bool shovelerCollidersAddCollider3(ShovelerColliders *colliders, struct ShovelerCollider3Struct *collider);
bool shovelerCollidersRemoveCollider2(ShovelerColliders *colliders, struct ShovelerCollider2Struct *collider);
bool shovelerCollidersRemoveCollider3(ShovelerColliders *colliders, struct ShovelerCollider3Struct *collider);
/** Intersects a 2d bounding box with colliders, returning the first intersecting collider. */
struct ShovelerCollider2Struct *shovelerCollidersIntersect2(ShovelerColliders *colliders, const ShovelerBoundingBox2 *boundingBox);
/** Intersects a 3d bounding box with colliders, returning the first intersecting collider. */
struct ShovelerCollider3Struct *shovelerCollidersIntersect3(ShovelerColliders *colliders, const ShovelerBoundingBox3 *boundingBox);
void shovelerCollidersFree(ShovelerColliders *colliders);

#endif
