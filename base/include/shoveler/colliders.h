#ifndef SHOVELER_COLLIDERS_H
#define SHOVELER_COLLIDERS_H

#include <glib.h>
#include <shoveler/collider.h>
#include <shoveler/types.h>

typedef struct ShovelerCollidersStruct {
  /** Hash set of (ShovelerCollider2 *) */
  /* private */ GHashTable* colliders2;
  /** Hash set of (ShovelerCollider3 *) */
  /* private */ GHashTable* colliders3;
} ShovelerColliders;

ShovelerColliders* shovelerCollidersCreate();
/** Adds a 2d collider to the colliders, with the caller retaining ownership over it and changes to
 * it being reflected live. */
bool shovelerCollidersAddCollider2(ShovelerColliders* colliders, ShovelerCollider2* collider);
/** Adds a 3d collider to the colliders, with the caller retaining ownership over it and changes to
 * it being reflected live. */
bool shovelerCollidersAddCollider3(ShovelerColliders* colliders, ShovelerCollider3* collider);
bool shovelerCollidersRemoveCollider2(ShovelerColliders* colliders, ShovelerCollider2* collider);
bool shovelerCollidersRemoveCollider3(ShovelerColliders* colliders, ShovelerCollider3* collider);
/** Intersects a 2d bounding box with colliders, returning the first intersecting collider. */
const ShovelerCollider2* shovelerCollidersIntersect2Filtered(
    ShovelerColliders* colliders,
    const ShovelerBoundingBox2* boundingBox,
    ShovelerCollider2FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData);
/** Intersects a 3d bounding box with colliders, returning the first intersecting collider. */
const ShovelerCollider3* shovelerCollidersIntersect3Filtered(
    ShovelerColliders* colliders,
    const ShovelerBoundingBox3* boundingBox,
    ShovelerCollider3FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData);
void shovelerCollidersFree(ShovelerColliders* colliders);

static inline const ShovelerCollider2* shovelerCollidersIntersect2(
    ShovelerColliders* colliders, const ShovelerBoundingBox2* boundingBox) {
  return shovelerCollidersIntersect2Filtered(
      colliders, boundingBox, /* filterCandidate */ NULL, /* filterCandidateUserData */ NULL);
}

static inline const ShovelerCollider3* shovelerCollidersIntersect3(
    ShovelerColliders* colliders, const ShovelerBoundingBox3* boundingBox) {
  return shovelerCollidersIntersect3Filtered(
      colliders, boundingBox, /* filterCandidate */ NULL, /* filterCandidateUserData */ NULL);
}

#endif
