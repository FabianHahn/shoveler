#include <stdlib.h> // malloc free
#include <string.h> // memcpy memset

#include "shoveler/collider.h"
#include "shoveler/tile_colliders.h"

static bool intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object);

ShovelerTileColliders *shovelerTileCollidersCreate(unsigned int numColumns, unsigned int numRows)
{
	ShovelerTileColliders *colliders = malloc(sizeof(ShovelerTileColliders));
	colliders->numColumns = numColumns;
	colliders->numRows = numRows;
	colliders->tiles = malloc(colliders->numColumns * colliders->numRows * sizeof(bool));
	memset(colliders->tiles, 0, colliders->numColumns * colliders->numRows * sizeof(bool));

	return colliders;
}

ShovelerCollider2 *shovelerTileCollidersCreateCollider(ShovelerTileColliders *colliders, ShovelerVector2 position, ShovelerVector2 size)
{
	ShovelerCollider2 *collider = malloc(sizeof(ShovelerCollider2));
	collider->boundingBox = shovelerBoundingBox2(
			shovelerVector2LinearCombination(1.0f, position, -0.5f, size),
			shovelerVector2LinearCombination(1.0f, position, 0.5f, size));
	collider->intersect = intersectCollider;
	collider->data = colliders;

	return collider;
}

void shovelerTileCollidersSet(ShovelerTileColliders *colliders, const bool *tiles)
{
	memcpy(colliders->tiles, tiles, colliders->numColumns * colliders->numRows * sizeof(bool));
}

void shovelerTileCollidersFree(ShovelerTileColliders *colliders)
{
	free(colliders->tiles);
	free(colliders);
}

static bool intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object)
{
	ShovelerTileColliders *colliders = collider->data;

	ShovelerVector2 size = shovelerVector2LinearCombination(1.0f, collider->boundingBox.max, -1.0f, collider->boundingBox.min);

	float columnStride = size.values[0] / colliders->numColumns;
	float rowStride = size.values[1] / colliders->numRows;

	for(unsigned int row = 0; row < colliders->numRows; row++) {
		unsigned int rowIndex = row * colliders->numRows;
		float rowCoordinate = collider->boundingBox.min.values[1] + row * rowStride;

		for(unsigned int column = 0; column < colliders->numColumns; column++) {
			if(!colliders->tiles[rowIndex + column]) {
				continue;
			}

			float columnCoordinate = collider->boundingBox.min.values[0] + column * columnStride;

			ShovelerBoundingBox2 tileBoundingBox = shovelerBoundingBox2(
					shovelerVector2(columnCoordinate, rowCoordinate),
					shovelerVector2(columnCoordinate + columnStride, rowCoordinate + rowStride));
			if(shovelerBoundingBox2Intersect(object, &tileBoundingBox)) {
				return true;
			}
		}
	}

	return false;
}
