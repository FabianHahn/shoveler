#include <stdlib.h> // malloc free
#include <string.h> // memcpy memset

#include "shoveler/collider/tilemap.h"
#include "shoveler/collider.h"

typedef struct {
	ShovelerCollider2 collider;
	unsigned int numColumns;
	unsigned int numRows;
	/** array of tiles, where tile (column, row) is at position [row * numColumns + column] */
	bool *tiles;
} ColliderData;

static ShovelerCollider2 *intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object);
static void freeCollider(ShovelerCollider2 *collider);

ShovelerCollider2 *shovelerColliderTilemapCreate(ShovelerBoundingBox2 boundingBox, unsigned int numColumns, unsigned int numRows)
{
	ColliderData *colliders = malloc(sizeof(ColliderData));
	colliders->collider.boundingBox = boundingBox;
	colliders->collider.data = colliders;
	colliders->collider.intersect = intersectCollider;
	colliders->collider.freeData = freeCollider;
	colliders->numColumns = numColumns;
	colliders->numRows = numRows;
	colliders->tiles = malloc(colliders->numColumns * colliders->numRows * sizeof(bool));
	memset(colliders->tiles, 0, colliders->numColumns * colliders->numRows * sizeof(bool));

	return &colliders->collider;
}

void shovelerColliderTilemapEnableTile(struct ShovelerCollider2Struct *collider, unsigned int column, unsigned int row)
{
	ColliderData *colliderData = collider->data;

	colliderData->tiles[row * colliderData->numColumns + column] = true;
}

void shovelerColliderTilemapDisableTile(struct ShovelerCollider2Struct *collider, unsigned int column, unsigned int row)
{
	ColliderData *colliderData = collider->data;

	colliderData->tiles[row * colliderData->numColumns + column] = false;
}

void shovelerColliderTilemapSetTiles(ShovelerCollider2 *collider, const bool *tiles)
{
	ColliderData *colliderData = collider->data;

	memcpy(colliderData->tiles, tiles, colliderData->numColumns * colliderData->numRows * sizeof(bool));
}

static ShovelerCollider2 *intersectCollider(ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object)
{
	ColliderData *colliders = collider->data;

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
				return collider;
			}
		}
	}

	return false;
}

static void freeCollider(ShovelerCollider2 *collider)
{
	ColliderData *colliderData = collider->data;

	free(colliderData->tiles);
	free(colliderData);
}
