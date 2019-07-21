#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/chunk.h"
#include "shoveler/image.h"
#include "shoveler/texture.h"
#include "shoveler/tilemap.h"
}

class ShovelerChunkTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		memset(collidingTiles, 0, 10 * 10 * sizeof(bool));
		collidingTiles[1 * 10 + 1] = true;
		collidingTiles[1 * 10 + 9] = true;
		collidingTiles[9 * 10 + 1] = true;
		collidingTiles[9 * 10 + 9] = true;

		tilesData = shovelerImageCreate(10, 10, 3);
		tiles.image = tilesData;

		tilemap = shovelerTilemapCreate(&tiles, collidingTiles);

		chunk = shovelerChunkCreate(shovelerVector2(5.0f, 5.0f), shovelerVector2(10.0f, 10.0f));
		shovelerChunkAddTilemapLayer(chunk, tilemap);
	}

	virtual void TearDown()
	{
		shovelerChunkFree(chunk);
		shovelerTilemapFree(tilemap);
		shovelerImageFree(tilesData);
	}

	bool collidingTiles[10 * 10];
	ShovelerImage *tilesData;
	ShovelerTexture tiles;
	ShovelerTilemap *tilemap;
	ShovelerChunk *chunk;
};

TEST_F(ShovelerChunkTest, intersection)
{
	ShovelerBoundingBox2 intersectingBox = shovelerBoundingBox2(
			shovelerVector2(0.0f, 0.0f),
			shovelerVector2(5.0f, 5.0f));
	bool intersects = shovelerChunkIntersect(chunk, &intersectingBox);
	ASSERT_TRUE(intersects);

	ShovelerBoundingBox2 intersectingBox2 = shovelerBoundingBox2(
			shovelerVector2(5.0f, 0.0f),
			shovelerVector2(10.0f, 5.0f));
	bool intersects2 = shovelerChunkIntersect(chunk, &intersectingBox2);
	ASSERT_TRUE(intersects2);

	ShovelerBoundingBox2 intersectingBox3 = shovelerBoundingBox2(
			shovelerVector2(0.0f, 5.0f),
			shovelerVector2(5.0f, 10.0f));
	bool intersects3 = shovelerChunkIntersect(chunk, &intersectingBox3);
	ASSERT_TRUE(intersects3);

	ShovelerBoundingBox2 intersectingBox4 = shovelerBoundingBox2(
			shovelerVector2(5.0f, 5.0f),
			shovelerVector2(10.0f, 10.0f));
	bool intersects4 = shovelerChunkIntersect(chunk, &intersectingBox4);
	ASSERT_TRUE(intersects4);
}

TEST_F(ShovelerChunkTest, noIntersection)
{
	ShovelerBoundingBox2 notIntersectingBox = shovelerBoundingBox2(
			shovelerVector2(2.5f, 2.5f),
			shovelerVector2(7.5f, 7.5f));
	bool intersects = shovelerChunkIntersect(chunk, &notIntersectingBox);
	ASSERT_FALSE(intersects);
}

TEST_F(ShovelerChunkTest, disableThenIntersect)
{
	collidingTiles[1 * 10 + 1] = false;

	ShovelerBoundingBox2 noLongerIntersectingBox = shovelerBoundingBox2(
			shovelerVector2(0.0f, 0.0f),
			shovelerVector2(5.0f, 5.0f));
	bool intersects = shovelerChunkIntersect(chunk, &noLongerIntersectingBox);
	ASSERT_FALSE(intersects);
}
