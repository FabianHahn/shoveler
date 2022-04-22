#include "shoveler/map.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <shoveler/log.h>

static void createStartingAreaBottomLeft(int chunkSize, ShovelerMapChunk* outputChunk);
static void createStartingAreaBottomRight(int chunkSize, ShovelerMapChunk* outputChunk);
static void createStartingAreaTopLeft(int chunkSize, ShovelerMapChunk* outputChunk);
static void createStartingAreaTopRight(int chunkSize, ShovelerMapChunk* outputChunk);
static void createChunk(int x, int y, int halfMapHeight, int halfMapWidth, int chunkSize, ShovelerMapChunk* outputChunk);

bool shovelerMapEntityHandleToChunkCoordinates(
	int64_t entityHandle,
	int64_t firstChunkEntityHandle,
	ShovelerMapDimensions dimensions,
	int* outputChunkX,
	int* outputChunkY)
{
	int diff = (int) (entityHandle - firstChunkEntityHandle);
	if (diff < 0 || diff >= dimensions.numChunkRows * dimensions.numChunkColumns) {
		return false;
	}

	*outputChunkX = diff / dimensions.numChunkRows;
	*outputChunkY = diff - *outputChunkX * dimensions.numChunkRows;
	return true;
}

ShovelerMap* shovelerMapGenerate(int chunkSize, int numChunkRows, int numChunkColumns)
{
	ShovelerMap* map = malloc(sizeof(ShovelerMap));
	map->dimensions = shovelerMapDimensions(chunkSize, numChunkRows, numChunkColumns);
	map->chunks = malloc(numChunkRows * numChunkColumns * sizeof(ShovelerMapChunk));

	for (int chunkX = 0; chunkX < numChunkColumns; chunkX++) {
		int x = -map->dimensions.halfMapWidth + chunkX * chunkSize;

		for (int chunkY = 0; chunkY < numChunkRows; chunkY++) {
			int y = -map->dimensions.halfMapHeight + chunkY * chunkSize;
			ShovelerMapChunk* chunk = shovelerMapGetChunk(map, chunkX, chunkY);
			shovelerMapChunkInit(chunk, chunkSize);

			if (x == -chunkSize && y == -chunkSize) {
				createStartingAreaBottomLeft(chunkSize, chunk);
			} else if (x == 0 && y == -chunkSize) {
				createStartingAreaBottomRight(chunkSize, chunk);
			} else if (x == -chunkSize && y == 0) {
				createStartingAreaTopLeft(chunkSize, chunk);
			} else if (x == 0 && y == 0) {
				createStartingAreaTopRight(chunkSize, chunk);
			} else {
				createChunk(
					x,
					y,
					map->dimensions.halfMapHeight,
					map->dimensions.halfMapWidth,
					chunkSize,
					chunk);
			}
		}
	}

	return map;
}

bool shovelerMapCheckTileCollision(ShovelerMap* map, ShovelerVector2 position, ShovelerVector2 size)
{
	ShovelerVector2 topRight = position;
	topRight.values[0] += 0.5f * size.values[0];
	topRight.values[1] += 0.5f * size.values[1];
	if (shovelerMapCheckPointCollision(map, topRight)) {
		return true;
	}

	ShovelerVector2 topLeft = position;
	topLeft.values[0] -= 0.5f * size.values[0];
	topLeft.values[1] += 0.5f * size.values[1];
	if (shovelerMapCheckPointCollision(map, topLeft)) {
		return true;
	}

	ShovelerVector2 bottomRight = position;
	bottomRight.values[0] += 0.5f * size.values[0];
	bottomRight.values[1] -= 0.5f * size.values[1];
	if (shovelerMapCheckPointCollision(map, bottomRight)) {
		return true;
	}

	ShovelerVector2 bottomLeft = position;
	bottomLeft.values[0] -= 0.5f * size.values[0];
	bottomLeft.values[1] -= 0.5f * size.values[1];
	if (shovelerMapCheckPointCollision(map, bottomLeft)) {
		return true;
	}

	return false;
}

bool shovelerMapCheckPointCollision(ShovelerMap* map, ShovelerVector2 position)
{
	ShovelerMapTileCoordinate tile = shovelerMapWorldToTile(&map->dimensions, position);

	if (!shovelerMapIsTileInBounds(&map->dimensions, tile)) {
		shovelerLogWarning(
			"Position (%.2f, %.2f) is not on colliding because tile coordinates are invalid.",
			position.values[0],
			position.values[1]);
		return false;
	}

	ShovelerMapTileData tileData = shovelerMapLookupTile(map, tile, /* background */ true);
	return tileData.tilesetCollider == (unsigned char) true;
}

ShovelerMapTileData shovelerMapLookupTile(
	ShovelerMap* map, ShovelerMapTileCoordinate tile, bool background)
{
	assert(shovelerMapIsTileInBounds(&map->dimensions, tile));

	ShovelerMapChunk* chunk = shovelerMapGetChunk(map, tile.chunkX, tile.chunkY);
	return shovelerMapChunkLookupTile(chunk, map->dimensions.chunkSize, tile.tileX, tile.tileY, background);
}

void shovelerMapWriteTile(
	ShovelerMap* map,
	ShovelerMapTileCoordinate tile,
	bool background,
	ShovelerMapTileData tileData)
{
	assert(shovelerMapIsTileInBounds(&map->dimensions, tile));

	ShovelerMapChunk* chunk = shovelerMapGetChunk(map, tile.chunkX, tile.chunkY);
	shovelerMapChunkWriteTile(chunk, map->dimensions.chunkSize, tile.tileX, tile.tileY, background, tileData);
}

ShovelerMapChunk* shovelerMapGetChunk(ShovelerMap* map, int chunkX, int chunkY)
{
	assert(chunkX >= 0);
	assert(chunkX < map->dimensions.numChunkColumns);
	assert(chunkY >= 0);
	assert(chunkY < map->dimensions.numChunkRows);
	return &map->chunks[chunkX * map->dimensions.numChunkRows + chunkY];
}

void shovelerMapFree(ShovelerMap* map)
{
	for (int i = 0; i < map->dimensions.numChunkRows * map->dimensions.numChunkColumns; i++) {
		shovelerMapChunkClear(&map->chunks[i]);
	}

	free(map->chunks);
	free(map);
}

static void createStartingAreaBottomLeft(int chunkSize, ShovelerMapChunk* outputChunk)
{
	outputChunk->position = shovelerVector2((float) -chunkSize / 2.0f, (float) -chunkSize / 2.0f);

	for (int tileY = 0; tileY < chunkSize; tileY++) {
		for (int tileX = 0; tileX < chunkSize; tileX++) {
			ShovelerMapTileData background = shovelerMapTileData();
			ShovelerMapTileData foreground = shovelerMapTileData();

			// fill background with grass
			int grassColumn = rand() % 3;
			int grassRow = rand() % 2;
			background.tilesetColumn = grassColumn;
			background.tilesetRow = grassRow;
			background.tilesetId = 2;
			background.tilesetCollider = false;

			// place boundary rocks
			if ((tileY == 0 || tileX == 0) && tileY != chunkSize - 1 && tileX != chunkSize - 1) {
				background.tilesetColumn = 5;
				background.tilesetRow = 0;
				background.tilesetCollider = true;
			}

			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ true, background);
			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ false, foreground);
		}
	}
}

static void createStartingAreaBottomRight(int chunkSize, ShovelerMapChunk* outputChunk)
{
	outputChunk->position = shovelerVector2((float) chunkSize / 2.0f, (float) -chunkSize / 2.0f);

	for (int tileY = 0; tileY < chunkSize; tileY++) {
		for (int tileX = 0; tileX < chunkSize; tileX++) {
			ShovelerMapTileData background = shovelerMapTileData();
			ShovelerMapTileData foreground = shovelerMapTileData();

			foreground.tilesetColumn = 0;
			foreground.tilesetRow = 0;
			foreground.tilesetId = 0;
			foreground.tilesetCollider = false;

			// fill background with grass
			int grassColumn = rand() % 3;
			int grassRow = rand() % 2;
			background.tilesetColumn = grassColumn;
			background.tilesetRow = grassRow;
			background.tilesetId = 2;
			background.tilesetCollider = false;

			// place boundary rocks
			if ((tileY == 0 || tileX == chunkSize - 1) && tileY != chunkSize - 1 && tileX != 0) {
				background.tilesetColumn = 5;
				background.tilesetRow = 0;
				background.tilesetCollider = true;
			}

			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ true, background);
			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ false, foreground);
		}
	}
}

static void createStartingAreaTopLeft(int chunkSize, ShovelerMapChunk* outputChunk)
{
	outputChunk->position = shovelerVector2((float) -chunkSize / 2.0f, (float) chunkSize / 2.0f);

	for (int tileY = 0; tileY < chunkSize; tileY++) {
		for (int tileX = 0; tileX < chunkSize; tileX++) {
			ShovelerMapTileData background = shovelerMapTileData();
			ShovelerMapTileData foreground = shovelerMapTileData();

			foreground.tilesetColumn = 0;
			foreground.tilesetRow = 0;
			foreground.tilesetId = 0;
			foreground.tilesetCollider = false;

			// fill background with grass
			int grassColumn = rand() % 3;
			int grassRow = rand() % 2;
			background.tilesetColumn = grassColumn;
			background.tilesetRow = grassRow;
			background.tilesetId = 2;
			foreground.tilesetCollider = false;

			// place boundary rocks
			if ((tileY == chunkSize - 1 || tileX == 0) && tileY != 0 && tileX != chunkSize - 1) {
				background.tilesetColumn = 5;
				background.tilesetRow = 0;
				background.tilesetCollider = true;
			}

			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ true, background);
			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ false, foreground);
		}
	}
}

static void createStartingAreaTopRight(int chunkSize, ShovelerMapChunk* outputChunk)
{
	outputChunk->position = shovelerVector2((float) chunkSize / 2.0f, (float) chunkSize / 2.0f);

	for (int tileY = 0; tileY < chunkSize; tileY++) {
		for (int tileX = 0; tileX < chunkSize; tileX++) {
			ShovelerMapTileData background = shovelerMapTileData();
			ShovelerMapTileData foreground = shovelerMapTileData();

			foreground.tilesetColumn = 0;
			foreground.tilesetRow = 0;
			foreground.tilesetId = 0;
			foreground.tilesetCollider = false;

			// fill background with grass
			int grassColumn = rand() % 3;
			int grassRow = rand() % 2;
			background.tilesetColumn = grassColumn;
			background.tilesetRow = grassRow;
			background.tilesetId = 2;
			foreground.tilesetCollider = false;

			// place boundary rocks
			if ((tileY == chunkSize - 1 || tileX == chunkSize - 1) && tileY != 0 && tileX != 0) {
				background.tilesetColumn = 5;
				background.tilesetRow = 0;
				background.tilesetCollider = true;
			}

			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ true, background);
			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ false, foreground);
		}
	}
}

static void createChunk(int x, int y, int halfMapHeight, int halfMapWidth, int chunkSize, ShovelerMapChunk* outputChunk)
{
	outputChunk->position = shovelerVector2(
		(float) x + (float) chunkSize / 2.0f,
		(float) y + (float) chunkSize / 2.0f);

	int rockSeedModulo = 5 + (rand() % 100);
	int bushSeedModulo = 5 + (rand() % 25);
	int treeSeedModulo = 5 + (rand() % 50);

	for (int tileY = 0; tileY < chunkSize; tileY++) {
		bool isBottomBorder = y == -halfMapHeight && tileY == 0;
		bool isTopBorder = y == halfMapHeight - chunkSize && tileY == chunkSize - 1;
		for (int tileX = 0; tileX < chunkSize; tileX++) {
			bool isLeftBorder = x == -halfMapWidth && tileX == 0;
			bool isRightBorder = x == halfMapWidth - chunkSize && tileX == chunkSize - 1;
			ShovelerMapTileData background = shovelerMapTileData();
			ShovelerMapTileData foreground = shovelerMapTileData();

			// map borders
			if (isLeftBorder || isRightBorder || isBottomBorder || isTopBorder) {
				// rock
				background.tilesetColumn = (unsigned char) 5;
				background.tilesetRow = (unsigned char) 0;
				background.tilesetId = (unsigned char) 2;
				background.tilesetCollider = (unsigned char) true;

				// nothing
				foreground.tilesetColumn = (unsigned char) 0;
				foreground.tilesetRow = (unsigned char) 0;
				foreground.tilesetId = (unsigned char) 0;
				foreground.tilesetCollider = (unsigned char) false;

				shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ true, background);
				shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ false, foreground);
				continue;
			}

			// fill background with grass
			int grassColumn = rand() % 3;
			int grassRow = rand() % 2;
			background.tilesetColumn = grassColumn;
			background.tilesetRow = grassRow;
			background.tilesetId = 2;
			foreground.tilesetCollider = false;

			// place rocks
			int rockSeed = rand() % rockSeedModulo;
			if (rockSeed == 0) {
				background.tilesetColumn = 5;
				background.tilesetRow = 0;
				background.tilesetCollider = true;
			}

			// place bushes
			int bushSeed = rand() % bushSeedModulo;
			if (bushSeed == 0) {
				background.tilesetColumn = 5;
				background.tilesetRow = 1;
				background.tilesetCollider = true;
			}

			// place trees
			do {
				if (tileY > 0) {
					ShovelerMapTileData previousRowBackground =
						shovelerMapChunkLookupTile(outputChunk, chunkSize, tileX, tileY - 1, /* background */ true);
					// complete left part of tree
					if (previousRowBackground.tilesetColumn == 3 && previousRowBackground.tilesetRow == 0) {
						foreground.tilesetColumn = 3;
						foreground.tilesetRow = 1;
						foreground.tilesetId = 2;
						break;
					}

					// complete right part of tree
					if (previousRowBackground.tilesetColumn == 4 && previousRowBackground.tilesetRow == 0) {
						foreground.tilesetColumn = 4;
						foreground.tilesetRow = 1;
						foreground.tilesetId = 2;
						break;
					}
				}

				if (tileX > 0) {
					ShovelerMapTileData previousTileBackground =
						shovelerMapChunkLookupTile(outputChunk, chunkSize, tileX - 1, tileY, /* background */ true);
					// complete bottom part of tree
					if (previousTileBackground.tilesetColumn == 3 && previousTileBackground.tilesetRow == 0) {
						background.tilesetColumn = 4;
						background.tilesetRow = 0;
						background.tilesetCollider = true;
						break;
					}
				}

				if (tileY > 0 && tileX < chunkSize - 1) {
					ShovelerMapTileData previousRowNextTileBackground =
						shovelerMapChunkLookupTile(outputChunk, chunkSize, tileX + 1, tileY - 1, /* background */ true);
					// reject positions that already have a tree starting on the bottom right neighbor tile
					if (previousRowNextTileBackground.tilesetColumn == 3 && previousRowNextTileBackground.tilesetRow == 0) {
						break;
					}
				}

				// reject positions that don't fit into the chunk
				if (tileY == chunkSize - 1 || tileX == chunkSize - 1) {
					break;
				}

				// start new tree
				int treeSeed = rand() % treeSeedModulo;
				if (treeSeed == 0) {
					background.tilesetColumn = 3;
					background.tilesetRow = 0;
					background.tilesetCollider = true;
				}
			} while (false);

			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ true, background);
			shovelerMapChunkWriteTile(outputChunk, chunkSize, tileX, tileY, /* background */ false, foreground);
		}
	}
}
