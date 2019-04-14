#ifndef SHOVELER_COLLIDER_H
#define SHOVELER_COLLIDER_H

#include <stdbool.h>

#include <glib.h>

#include <shoveler/types.h>

struct ShovelerCollider2Struct; // forward declaration: below
struct ShovelerCollider3Struct; // forward declaration: below

typedef struct ShovelerCollider2Struct *(ShovelerCollider2IntersectFunction)(struct ShovelerCollider2Struct *collider, const ShovelerBoundingBox2 *object);
typedef void (ShovelerCollider2FreeDataFunction)(struct ShovelerCollider2Struct *collider);
typedef struct ShovelerCollider3Struct *(ShovelerCollider3IntersectFunction)(struct ShovelerCollider3Struct *collider, const ShovelerBoundingBox3 *object);
typedef void (ShovelerCollider3FreeDataFunction)(struct ShovelerCollider3Struct *collider);

typedef struct ShovelerCollider2Struct {
	ShovelerBoundingBox2 boundingBox;
	void *data;
	ShovelerCollider2IntersectFunction *intersect;
	ShovelerCollider2FreeDataFunction *freeData;
} ShovelerCollider2;

typedef struct ShovelerCollider3Struct {
	ShovelerBoundingBox3 boundingBox;
	void *data;
	ShovelerCollider3IntersectFunction *intersect;
	ShovelerCollider3FreeDataFunction *freeData;
} ShovelerCollider3;

static inline ShovelerCollider2 *shovelerCollider2Intersect(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object)
{
	if(shovelerBoundingBox2Intersect(&collider->boundingBox, object)) {
		if(collider->intersect != NULL) {
			return collider->intersect(collider, object);
		}
		return collider;
	}
	return NULL;
}

static inline void shovelerCollider2Free(ShovelerCollider2 *collider)
{
	if(collider == NULL) {
		return;
	}

	if(collider->freeData != NULL) {
		collider->freeData(collider);
	}
}

static inline ShovelerCollider3 *shovelerCollider3Intersect(ShovelerCollider3 *collider, const ShovelerBoundingBox3 *object)
{
	if(shovelerBoundingBox3Intersect(&collider->boundingBox, object)) {
		if(collider->intersect != NULL) {
			return collider->intersect(collider, object);
		}
		return collider;
	}
	return NULL;
}

static inline void shovelerCollider3Free(ShovelerCollider3 *collider)
{
	if(collider == NULL) {
		return;
	}

	if(collider->freeData != NULL) {
		collider->freeData(collider);
	}
}

#endif
