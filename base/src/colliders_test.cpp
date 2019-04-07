#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/collider/box.h"
#include "shoveler/colliders.h"
}

class ShovelerCollidersTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		colliders = shovelerCollidersCreate();
	}

	virtual void TearDown()
	{
		shovelerCollidersFree(colliders);
	}

	ShovelerColliders *colliders;
};

TEST_F(ShovelerCollidersTest, intersect2)
{
	ShovelerCollider2 collider = shovelerColliderBox2(shovelerBoundingBox2(
			shovelerVector2(0.0f, 0.0f),
			shovelerVector2(5.0f, 5.0f)));
	ShovelerCollider2 collider2 = shovelerColliderBox2(shovelerBoundingBox2(
			shovelerVector2(-5.0f, -5.0f),
			shovelerVector2(0.0f, 0.0f)));
	shovelerCollidersAddCollider2(colliders, &collider);
	shovelerCollidersAddCollider2(colliders, &collider2);

	ShovelerBoundingBox2 intersectingBox = shovelerBoundingBox2(
			shovelerVector2(2.5f, 2.5f),
			shovelerVector2(7.5f, 7.5f));
	ShovelerCollider2 *intersectingCollider = shovelerCollidersIntersect2(colliders, &intersectingBox);
	ASSERT_EQ(intersectingCollider, &collider);

	ShovelerBoundingBox2 intersectingBox2 = shovelerBoundingBox2(
			shovelerVector2(-7.5f, -7.5f),
			shovelerVector2(2.5f, 2.5f));
	ShovelerCollider2 *intersectingCollider2 = shovelerCollidersIntersect2(colliders, &intersectingBox2);
	ASSERT_EQ(intersectingCollider2, &collider2);

	ShovelerBoundingBox2 eitherIntersectingBox = shovelerBoundingBox2(
			shovelerVector2(-2.5f, -2.5f),
			shovelerVector2(2.5f, 2.5f));
	ShovelerCollider2 *eitherIntersectingCollider = shovelerCollidersIntersect2(colliders, &eitherIntersectingBox);
	ASSERT_TRUE(eitherIntersectingCollider == &collider || eitherIntersectingCollider == &collider2);

	ShovelerBoundingBox2 notIntersectingBox = shovelerBoundingBox2(
			shovelerVector2(-7.5f, 2.5f),
			shovelerVector2(-2.5f, 7.5f));
	ShovelerCollider2 *notIntersectingCollider = shovelerCollidersIntersect2(colliders, &notIntersectingBox);
	ASSERT_TRUE(notIntersectingCollider == NULL);
}

TEST_F(ShovelerCollidersTest, intersect3)
{
	ShovelerCollider3 collider = shovelerColliderBox3(shovelerBoundingBox3(
			shovelerVector3(0.0f, 0.0f, 0.0f),
			shovelerVector3(5.0f, 5.0f, 5.0f)));
	ShovelerCollider3 collider2 = shovelerColliderBox3(shovelerBoundingBox3(
			shovelerVector3(-5.0f, -5.0f, -5.0f),
			shovelerVector3(0.0f, 0.0f, 0.0f)));
	shovelerCollidersAddCollider3(colliders, &collider);
	shovelerCollidersAddCollider3(colliders, &collider2);

	ShovelerBoundingBox3 intersectingBox = shovelerBoundingBox3(
			shovelerVector3(2.5f, 2.5f, 2.5f),
			shovelerVector3(7.5f, 7.5f, 7.5f));
	ShovelerCollider3 *intersectingCollider = shovelerCollidersIntersect3(colliders, &intersectingBox);
	ASSERT_EQ(intersectingCollider, &collider);

	ShovelerBoundingBox3 intersectingBox2 = shovelerBoundingBox3(
			shovelerVector3(-7.5f, -7.5f, -7.5f),
			shovelerVector3(2.5f, 2.5f, 2.5f));
	ShovelerCollider3 *intersectingCollider2 = shovelerCollidersIntersect3(colliders, &intersectingBox2);
	ASSERT_EQ(intersectingCollider2, &collider2);

	ShovelerBoundingBox3 eitherIntersectingBox = shovelerBoundingBox3(
			shovelerVector3(-2.5f, -2.5f, -2.5f),
			shovelerVector3(2.5f, 2.5f, 2.5f));
	ShovelerCollider3 *eitherIntersectingCollider = shovelerCollidersIntersect3(colliders, &eitherIntersectingBox);
	ASSERT_TRUE(eitherIntersectingCollider == &collider || eitherIntersectingCollider == &collider2);

	ShovelerBoundingBox3 notIntersectingBox = shovelerBoundingBox3(
			shovelerVector3(-7.5f, 2.5f, 2.5f),
			shovelerVector3(-2.5f, 7.5f, 7.5f));
	ShovelerCollider3 *notIntersectingCollider = shovelerCollidersIntersect3(colliders, &notIntersectingBox);
	ASSERT_TRUE(notIntersectingCollider == NULL);
}
