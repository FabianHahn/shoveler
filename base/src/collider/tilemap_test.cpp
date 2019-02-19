#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/collider/box.h"
#include "shoveler/collider/tilemap.h"
}

class ShovelerColliderTilemapTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		ShovelerBoundingBox2 boundingBox2 = shovelerBoundingBox2(
			shovelerVector2(0.0f, 0.0f),
			shovelerVector2(10.0f, 10.0f));
		collider = shovelerColliderTilemapCreate(boundingBox2, 10, 10);
		shovelerColliderTilemapEnableTile(collider, 1, 1);
		shovelerColliderTilemapEnableTile(collider, 1, 9);
		shovelerColliderTilemapEnableTile(collider, 9, 1);
		shovelerColliderTilemapEnableTile(collider, 9, 9);
	}

	virtual void TearDown()
	{
		shovelerCollider2Free(collider);
	}

	ShovelerCollider2 *collider;
};

TEST_F(ShovelerColliderTilemapTest, intersection)
{
	ShovelerBoundingBox2 intersectingBox = shovelerBoundingBox2(
		shovelerVector2(0.0f, 0.0f),
		shovelerVector2(5.0f, 5.0f));
	bool intersects = shovelerCollider2Intersect(collider, &intersectingBox);
	ASSERT_TRUE(intersects);

	ShovelerBoundingBox2 intersectingBox2 = shovelerBoundingBox2(
			shovelerVector2(5.0f, 0.0f),
			shovelerVector2(10.0f, 5.0f));
	bool intersects2 = shovelerCollider2Intersect(collider, &intersectingBox2);
	ASSERT_TRUE(intersects2);

	ShovelerBoundingBox2 intersectingBox3 = shovelerBoundingBox2(
			shovelerVector2(0.0f, 5.0f),
			shovelerVector2(5.0f, 10.0f));
	bool intersects3 = shovelerCollider2Intersect(collider, &intersectingBox3);
	ASSERT_TRUE(intersects3);

	ShovelerBoundingBox2 intersectingBox4 = shovelerBoundingBox2(
			shovelerVector2(5.0f, 5.0f),
			shovelerVector2(10.0f, 10.0f));
	bool intersects4 = shovelerCollider2Intersect(collider, &intersectingBox4);
	ASSERT_TRUE(intersects4);
}

TEST_F(ShovelerColliderTilemapTest, noIntersection)
{
	ShovelerBoundingBox2 notIntersectingBox = shovelerBoundingBox2(
			shovelerVector2(2.5f, 2.5f),
			shovelerVector2(7.5f, 7.5f));
	bool intersects = shovelerCollider2Intersect(collider, &notIntersectingBox);
	ASSERT_FALSE(intersects);
}

TEST_F(ShovelerColliderTilemapTest, disableThenIntersect)
{
	shovelerColliderTilemapDisableTile(collider, 1, 1);

	ShovelerBoundingBox2 noLongerIntersectingBox = shovelerBoundingBox2(
			shovelerVector2(0.0f, 0.0f),
			shovelerVector2(5.0f, 5.0f));
	bool intersects = shovelerCollider2Intersect(collider, &noLongerIntersectingBox);
	ASSERT_FALSE(intersects);
}
