#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "shoveler/image.h"
#include "shoveler/texture.h"
#include "shoveler/tilemap.h"
}

class ShovelerTilemapTest : public ::testing::Test {
public:
  virtual void SetUp() {
    memset(collidingTiles, 0, 10 * 10 * sizeof(bool));
    collidingTiles[1 * 10 + 1] = true;
    collidingTiles[1 * 10 + 9] = true;
    collidingTiles[9 * 10 + 1] = true;
    collidingTiles[9 * 10 + 9] = true;

    tilesData = shovelerImageCreate(10, 10, 3);
    tiles.width = 10;
    tiles.height = 10;
    tiles.image = tilesData;

    tilemap = shovelerTilemapCreate(&tiles, collidingTiles);

    boundingBox = shovelerBoundingBox2(shovelerVector2(0.0f, 0.0f), shovelerVector2(10.0f, 10.0f));
  }

  virtual void TearDown() {
    shovelerTilemapFree(tilemap);
    shovelerImageFree(tilesData);
  }

  bool collidingTiles[10 * 10];
  ShovelerImage* tilesData;
  ShovelerTexture tiles;
  ShovelerTilemap* tilemap;
  ShovelerBoundingBox2 boundingBox;
};

TEST_F(ShovelerTilemapTest, intersection) {
  ShovelerBoundingBox2 intersectingBox =
      shovelerBoundingBox2(shovelerVector2(0.0f, 0.0f), shovelerVector2(5.0f, 5.0f));
  bool intersects = shovelerTilemapIntersect(tilemap, &boundingBox, &intersectingBox);
  ASSERT_TRUE(intersects);

  ShovelerBoundingBox2 intersectingBox2 =
      shovelerBoundingBox2(shovelerVector2(5.0f, 0.0f), shovelerVector2(10.0f, 5.0f));
  bool intersects2 = shovelerTilemapIntersect(tilemap, &boundingBox, &intersectingBox2);
  ASSERT_TRUE(intersects2);

  ShovelerBoundingBox2 intersectingBox3 =
      shovelerBoundingBox2(shovelerVector2(0.0f, 5.0f), shovelerVector2(5.0f, 10.0f));
  bool intersects3 = shovelerTilemapIntersect(tilemap, &boundingBox, &intersectingBox3);
  ASSERT_TRUE(intersects3);

  ShovelerBoundingBox2 intersectingBox4 =
      shovelerBoundingBox2(shovelerVector2(5.0f, 5.0f), shovelerVector2(10.0f, 10.0f));
  bool intersects4 = shovelerTilemapIntersect(tilemap, &boundingBox, &intersectingBox4);
  ASSERT_TRUE(intersects4);
}

TEST_F(ShovelerTilemapTest, noIntersection) {
  ShovelerBoundingBox2 notIntersectingBox =
      shovelerBoundingBox2(shovelerVector2(2.5f, 2.5f), shovelerVector2(7.5f, 7.5f));
  bool intersects = shovelerTilemapIntersect(tilemap, &boundingBox, &notIntersectingBox);
  ASSERT_FALSE(intersects);
}

TEST_F(ShovelerTilemapTest, disableThenIntersect) {
  collidingTiles[1 * 10 + 1] = false;

  ShovelerBoundingBox2 noLongerIntersectingBox =
      shovelerBoundingBox2(shovelerVector2(0.0f, 0.0f), shovelerVector2(5.0f, 5.0f));
  bool intersects = shovelerTilemapIntersect(tilemap, &boundingBox, &noLongerIntersectingBox);
  ASSERT_FALSE(intersects);
}
