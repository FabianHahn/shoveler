#include <stdlib.h> // malloc free

#include "shoveler/chunk.h"
#include "shoveler/collider.h"
#include "shoveler/types.h"

static ShovelerCollider2 *intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object);
static void freeCollider(ShovelerCollider2 *collider);

ShovelerCollider2 *shovelerColliderChunkCreate(ShovelerChunk *chunk)
{
	ShovelerCollider2 *collider = malloc(sizeof(ShovelerCollider2));
	collider->boundingBox = shovelerBoundingBox2(
		shovelerVector2LinearCombination(1.0f, chunk->position, -0.5f, chunk->size),
		shovelerVector2LinearCombination(1.0f, chunk->position, 0.5f, chunk->size));
	collider->data = chunk;
	collider->intersect = intersectCollider;
	collider->freeData = freeCollider;
}

static ShovelerCollider2 *intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object)
{
	ShovelerChunk *chunk = collider->data;

	if(shovelerChunkIntersect(chunk, object)) {
		return collider;
	}

	return NULL;
}

static void freeCollider(ShovelerCollider2 *collider)
{
	free(collider);
}
