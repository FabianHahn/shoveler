#ifndef SHOVELER_COLLIDER_H
#define SHOVELER_COLLIDER_H

#include <glib.h>
#include <shoveler/types.h>
#include <stdbool.h>

typedef struct ShovelerCollider2Struct ShovelerCollider2; // forward declaration: below
typedef struct ShovelerCollider3Struct ShovelerCollider3; // forward declaration: below

typedef bool(ShovelerCollider2FilterCandidateFunction)(
    const ShovelerCollider2* candidate, void* userData);
typedef bool(ShovelerCollider3FilterCandidateFunction)(
    const ShovelerCollider3* candidate, void* userData);

typedef const ShovelerCollider2*(ShovelerCollider2IntersectFunction)(
    const ShovelerCollider2* collider,
    const ShovelerBoundingBox2* object,
    ShovelerCollider2FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData);
typedef const ShovelerCollider3*(ShovelerCollider3IntersectFunction)(
    const ShovelerCollider3* collider,
    const ShovelerBoundingBox3* object,
    ShovelerCollider3FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData);

typedef struct ShovelerCollider2Struct {
  ShovelerBoundingBox2 boundingBox;
  ShovelerCollider2IntersectFunction* intersect;
  void* data;
} ShovelerCollider2;

typedef struct ShovelerCollider3Struct {
  ShovelerBoundingBox3 boundingBox;
  ShovelerCollider3IntersectFunction* intersect;
  void* data;
} ShovelerCollider3;

const ShovelerCollider2* shovelerCollider2IntersectFiltered(
    const ShovelerCollider2* collider,
    const ShovelerBoundingBox2* object,
    ShovelerCollider2FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData);
const ShovelerCollider3* shovelerCollider3IntersectFiltered(
    const ShovelerCollider3* collider,
    const ShovelerBoundingBox3* object,
    ShovelerCollider3FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData);

static inline const ShovelerCollider2* shovelerCollider2Intersect(
    const ShovelerCollider2* collider, const ShovelerBoundingBox2* object) {
  return shovelerCollider2IntersectFiltered(
      collider, object, /* filterCandidate */ NULL, /* filterCandidateUserData */ NULL);
}

static inline const ShovelerCollider3* shovelerCollider3Intersect(
    const ShovelerCollider3* collider, const ShovelerBoundingBox3* object) {
  return shovelerCollider3IntersectFiltered(
      collider, object, /* filterCandidate */ NULL, /* filterCandidateUserData */ NULL);
}

#endif
