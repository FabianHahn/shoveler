#include <gtest/gtest.h>

#include "image_testing.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

extern "C" {
#include "shoveler/image.h"
#include "shoveler/image/png.h"
}

class ShovelerImagePngTest : public ::testing::Test {
public:
  virtual void SetUp() {
    remove(testFilename);

    testImage = shovelerImageCreate(2, 2, 3);
    shovelerImageClear(testImage);
    shovelerImageGet(testImage, 0, 0, 0) = 255;
    shovelerImageGet(testImage, 0, 1, 1) = 255;
    shovelerImageGet(testImage, 1, 0, 2) = 255;
    shovelerImageGet(testImage, 1, 1, 1) = 255;
    shovelerImageGet(testImage, 1, 1, 2) = 255;
  }

  virtual void TearDown() {
    shovelerImageFree(testImage);
    remove(testFilename);
  }

  const char* testFilename = "test.png";
  ShovelerImage* testImage;
};

TEST_F(ShovelerImagePngTest, writeReadFile) {
  bool written = shovelerImagePngWriteFile(testImage, testFilename);
  ASSERT_TRUE(written) << "png should be written successfully";

  ShovelerImage* image = shovelerImagePngReadFile(testFilename);
  ASSERT_TRUE(image != NULL) << "png should be read successfully";
  ASSERT_EQ(*image, *testImage) << "read image should be equal to test image";

  shovelerImageFree(image);
}

TEST_F(ShovelerImagePngTest, writeReadMemory) {
  bool written = shovelerImagePngWriteFile(testImage, testFilename);
  ASSERT_TRUE(written) << "png should be written successfully";

  std::ifstream file(testFilename, std::ios::binary);
  std::stringstream stream;
  stream << file.rdbuf();
  std::string contents(stream.str());

  ShovelerImage* image = shovelerImagePngReadBuffer(
      reinterpret_cast<const unsigned char*>(contents.c_str()), contents.size());
  ASSERT_TRUE(image != NULL) << "png should be read successfully";
  ASSERT_EQ(*image, *testImage) << "read image should be equal to test image";

  shovelerImageFree(image);
}
