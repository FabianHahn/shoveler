#include "shoveler/tiles/tileset.h"

#include "shoveler/image/png.h"
#include "shoveler/log.h"

static const int tileWidth = 16;
static const int tileHeight = 16;

static void addFrame(ShovelerImage *tile, int size);
static void setTile(ShovelerImage *tileset, int column, int row, ShovelerImage *tile);
static int getBorderDistance(int i, int j);
static int getMin(int a, int b);

void shovelerTilesCreateTileset(ShovelerImage **outputTileset, int *outputColumns, int *outputRows)
{
	int columns = 4;
	int rows = 1;
	ShovelerImage *tileset = shovelerImageCreate(columns * tileWidth, rows * tileHeight, 3);
	shovelerImageClear(tileset);

	ShovelerImage *tile = shovelerImageCreate(tileWidth, tileHeight, 3);

	shovelerImageClear(tile);
	addFrame(tile, 1);
	setTile(tileset, 0, 0, tile);

	shovelerImageClear(tile);
	addFrame(tile, 3);
	setTile(tileset, 1, 0, tile);

	shovelerImageClear(tile);
	addFrame(tile, 5);
	setTile(tileset, 2, 0, tile);

	shovelerImageClear(tile);
	addFrame(tile, 7);
	setTile(tileset, 3, 0, tile);

	shovelerImageFree(tile);

	*outputTileset = tileset;
	*outputColumns = columns;
	*outputRows = rows;
}

static void addFrame(ShovelerImage *tile, int size)
{
	for(int i = 0; i < tileWidth; i++) {
		for(int j = 0; j < tileHeight; j++) {
			int borderDistance = getBorderDistance(i, j);
			if(borderDistance < size) {
				unsigned char color = 255.0 * ((double) (size - borderDistance) / size);
				for(int c = 0; c < 3; c++) {
					shovelerImageGet(tile, i, j, c) = color;
				}
			}
		}
	}
}

static void setTile(ShovelerImage *tileset, int column, int row, ShovelerImage *tile)
{
	int xOffset = column * tileWidth;
	int yOffset = row * tileHeight;

	for(int i = 0; i < tileWidth; i++) {
		for(int j = 0; j < tileHeight; j++) {
			for(int c = 0; c < 3; c++) {
				shovelerImageGet(tileset, xOffset + i, yOffset + j, c) = shovelerImageGet(tile, i, j, c);
			}
		}
	}
}

static int getBorderDistance(int i, int j)
{
	int xLow = i;
	int xHigh = tileWidth - 1 - i;
	int yLow = j;
	int yHigh = tileHeight - 1 - j;
	return getMin(xLow, getMin(xHigh, getMin(yLow, yHigh)));
}

static int getMin(int a, int b)
{
	return a <= b ? a : b;
}
