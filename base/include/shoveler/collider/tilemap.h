#ifndef SHOVELER_COLLIDER_TILEMAP_H
#define SHOVELER_COLLIDER_TILEMAP_H

#include <stdbool.h> // bool

#include <shoveler/types.h>

struct ShovelerCollider2Struct; // forward declaration: collider.h

struct ShovelerCollider2Struct *shovelerColliderTilemapCreate(ShovelerBoundingBox2 boundingBox, unsigned int numColumns, unsigned int numRows);
void shovelerColliderTilemapEnableTile(struct ShovelerCollider2Struct *collider, unsigned int column, unsigned int row);
void shovelerColliderTilemapDisableTile(struct ShovelerCollider2Struct *collider, unsigned int column, unsigned int row);
/** Sets the collider's tiles, where tile (column, row) is at position [row * numColumns + column] */
void shovelerColliderTilemapSetTiles(struct ShovelerCollider2Struct *collider, const bool *tiles);

#endif
