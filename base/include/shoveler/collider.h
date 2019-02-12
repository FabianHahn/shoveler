#ifndef SHOVELER_COLLIDER_H
#define SHOVELER_COLLIDER_H

#include <glib.h>

#include <shoveler/types.h>

struct ShovelerCollider2Struct; // forward declaration: below
struct ShovelerCollider3Struct; // forward declaration: below

typedef bool (ShovelerCollider2IntersectionFunction)(struct ShovelerCollider2Struct *collider, const ShovelerBoundingBox2 *object);
typedef bool (ShovelerCollider3IntersectionFunction)(struct ShovelerCollider3Struct *collider, const ShovelerBoundingBox3 *object);

typedef struct ShovelerCollider2Struct {
	ShovelerBoundingBox2 boundingBox;
	void *data;
	ShovelerCollider2IntersectionFunction *intersect;
} ShovelerCollider2;

typedef struct ShovelerCollider3Struct {
	ShovelerBoundingBox3 boundingBox;
	void *data;
	ShovelerCollider3IntersectionFunction *intersect;
} ShovelerCollider3;

typedef struct ShovelerCollidersStruct {
	/** Hash set of (ShovelerCollider2 *) */
	/* private */ GHashTable *colliders2;
	/** Hash set of (ShovelerCollider3 *) */
	/* private */ GHashTable *colliders3;
} ShovelerColliders;

ShovelerColliders *shovelerCollidersCreate();
/** Adds a 2d collider to the colliders, with the caller retaining ownership over it and changes to it being reflected live. */
bool shovelerCollidersAddCollider2(ShovelerColliders *colliders, ShovelerCollider2 *collider);
/** Adds a 3d collider to the colliders, with the caller retaining ownership over it and changes to it being reflected live. */
bool shovelerCollidersAddCollider3(ShovelerColliders *colliders, ShovelerCollider3 *collider);
bool shovelerCollidersRemoveCollider2(ShovelerColliders *colliders, ShovelerCollider2 *collider);
bool shovelerCollidersRemoveCollider3(ShovelerColliders *colliders, ShovelerCollider3 *collider);
/** Intersects a 2d bounding box with colliders, returning the first intersecting collider. */
ShovelerCollider2 *shovelerCollidersIntersect2(ShovelerColliders *colliders, const ShovelerBoundingBox2 *boundingBox);
/** Intersects a 3d bounding box with colliders, returning the first intersecting collider. */
ShovelerCollider3 *shovelerCollidersIntersect3(ShovelerColliders *colliders, const ShovelerBoundingBox3 *boundingBox);
void shovelerCollidersFree(ShovelerColliders *colliders);

#endif
