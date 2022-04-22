#ifndef SHOVELER_MAP_DIMENSIONS_H
#define SHOVELER_MAP_DIMENSIONS_H

#include <shoveler/types.h>
#include <shoveler/position_quantizer.h>

typedef struct {
	int chunkSize;
	int numChunkRows;
	int numChunkColumns;
	int halfMapHeight;
	int halfMapWidth;
} ShovelerMapDimensions;

typedef struct {
	int chunkX;
	int chunkY;
	int tileX;
	int tileY;
} ShovelerMapTileCoordinate;

static inline ShovelerMapDimensions shovelerMapDimensions(
	int chunkSize, int numChunkRows, int numChunkColumns)
{
	ShovelerMapDimensions mapDimensions;
	mapDimensions.chunkSize = chunkSize;
	mapDimensions.numChunkRows = numChunkRows;
	mapDimensions.numChunkColumns = numChunkColumns;
	mapDimensions.halfMapHeight = numChunkRows * chunkSize / 2;
	mapDimensions.halfMapWidth = numChunkColumns * chunkSize / 2;
	return mapDimensions;
}

static inline ShovelerVector2 shovelerMapTileToWorld(
	const ShovelerMapDimensions* mapDimensions, ShovelerMapTileCoordinate tile)
{
	return shovelerVector2(
		(float) (-mapDimensions->halfMapWidth + tile.chunkX * mapDimensions->chunkSize + tile.tileX),
		(float) (-mapDimensions->halfMapHeight + tile.chunkY * mapDimensions->chunkSize + tile.tileY));
}

static inline ShovelerMapTileCoordinate shovelerMapWorldToTile(
	const ShovelerMapDimensions* mapDimensions, ShovelerVector2 position)
{
	float diffX = position.values[0] + (float) mapDimensions->halfMapWidth;
	float diffZ = position.values[1] + (float) mapDimensions->halfMapHeight;
	float chunkSizef = (float) mapDimensions->chunkSize;

	ShovelerMapTileCoordinate tile;
	tile.chunkX = (int) floorf(diffX / chunkSizef);
	tile.chunkY = (int) floorf(diffZ / chunkSizef);
	tile.tileX = (int) floorf(diffX - (float) tile.chunkX * chunkSizef);
	tile.tileY = (int) floorf(diffZ - (float) tile.chunkY * chunkSizef);
	return tile;
}

static inline bool shovelerMapIsTileInBounds(
	const ShovelerMapDimensions* mapDimensions, ShovelerMapTileCoordinate tile)
{
	return tile.chunkX >= 0 &&
		tile.chunkX < mapDimensions->numChunkColumns &&
		tile.chunkY >= 0 &&
		tile.chunkY < mapDimensions->numChunkRows &&
		tile.tileX >= 0 &&
		tile.tileX < mapDimensions->chunkSize &&
		tile.tileY >= 0 &&
		tile.tileY < mapDimensions->chunkSize;
}

static inline bool shovelerMapIsPositionInBounds(
	const ShovelerMapDimensions* mapDimensions, ShovelerVector2 position)
{
	return position.values[0] >= (float) -mapDimensions->halfMapWidth &&
		position.values[0] <= (float) mapDimensions->halfMapWidth &&
		position.values[1] >= (float) -mapDimensions->halfMapHeight &&
		position.values[1] <= (float) mapDimensions->halfMapHeight;
}

static inline ShovelerPositionQuantizer shovelerPositionQuantizerFromMapDimensions(
	const ShovelerMapDimensions* mapDimensions)
{
	return shovelerPositionQuantizer(
		/* worldMin */ shovelerVector2(
			-1.0f * (float) mapDimensions->halfMapWidth,
			-1.0f * (float) mapDimensions->halfMapHeight),
		/* worldSize */ shovelerVector2(
			2.0f * (float) mapDimensions->halfMapWidth,
			2.0f * (float) mapDimensions->halfMapHeight));
}

#endif
