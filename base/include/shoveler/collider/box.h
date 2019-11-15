#ifndef SHOVELER_COLLIDER_BOX_H
#define SHOVELER_COLLIDER_BOX_H

#include <shoveler/collider.h>
#include <shoveler/types.h>

static inline ShovelerCollider2 shovelerColliderBox2(ShovelerBoundingBox2 boundingBox)
{
	ShovelerCollider2 collider;
	collider.boundingBox = boundingBox;
	collider.data = NULL;
	collider.intersect = NULL;

	return collider;
}

static inline ShovelerCollider3 shovelerColliderBox3(ShovelerBoundingBox3 boundingBox)
{
	ShovelerCollider3 collider;
	collider.boundingBox = boundingBox;
	collider.data = NULL;
	collider.intersect = NULL;

	return collider;
}

#endif
