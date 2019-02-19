#ifndef SHOVELER_TILE_COLLIDERS_H
#define SHOVELER_TILE_COLLIDERS_H

#include <stdbool.h> // bool

#include <shoveler/types.h>

struct ShovelerColliderStruct; // forward declaration: collider.h

typedef struct ShovelerTileCollidersStruct {
	unsigned int numColumns;
	unsigned int numRows;
	/** array of tiles, where tile (column, row) is at position [row * numColumns + column] */
	bool *tiles;
} ShovelerTileColliders;

ShovelerTileColliders *shovelerTileCollidersCreate(unsigned int numColumns, unsigned int numRows);
struct ShovelerCollider2Struct *shovelerTileCollidersCreateCollider(ShovelerTileColliders *colliders, ShovelerVector2 position, ShovelerVector2 size);
void shovelerTileCollidersSet(ShovelerTileColliders *colliders, const bool *tiles);
void shovelerTileCollidersFree(ShovelerTileColliders *colliders);

#endif
