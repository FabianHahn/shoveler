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
#include "shoveler/image/png.h"
#include "shoveler/resources/image_png.h"
}

static void requestResources(
    ShovelerResources* resources, const char* typeId, const char* resourceId, void* testPointer);

class ShovelerResourcesImagePngTest : public ::testing::Test {
public:
  virtual void SetUp() {
    remove(testFilename);

    testImage = shovelerImageCreate(2, 2, 3);
    shovelerImageClear(testImage);

    shovelerImagePngWriteFile(testImage, testFilename);

    std::ifstream file(testFilename, std::ios::binary);
    std::stringstream stream;
    stream << file.rdbuf();
    testImageData = std::string(stream.str());

    resources = shovelerResourcesCreate(requestResources, this);
    bool registered = shovelerResourcesImagePngRegister(resources);
    ASSERT_TRUE(registered) << "image png type loader should have successfully registered";
  }

  virtual void TearDown() {
    shovelerImageFree(testImage);
    shovelerResourcesFree(resources);
    remove(testFilename);
  }

  const char* testFilename = "test.png";
  ShovelerImage* testImage;
  std::string testImageData;

  ShovelerResources* resources;
};

TEST_F(ShovelerResourcesImagePngTest, load) {
  const char* testResourceId = "test resource id";

  ShovelerResource* resource =
      shovelerResourcesGet(resources, shovelerResourcesImagePngTypeId, testResourceId);
  ASSERT_TRUE(resource != NULL) << "requesting resource should have been successful";

  ShovelerImage* resourceImage = (ShovelerImage*) resource->data;
  ASSERT_EQ(*resourceImage, *testImage) << "loaded resource image should be equal to test image";
}

TEST_F(ShovelerResourcesImagePngTest, loadFailed) {
  const char* testResourceId = "test resource id";

  testImageData.clear();
  ShovelerResource* resource =
      shovelerResourcesGet(resources, shovelerResourcesImagePngTypeId, testResourceId);
  ASSERT_TRUE(resource != NULL) << "requesting resource should have been successful";

  ShovelerImage* resourceImage = (ShovelerImage*) resource->data;
  ASSERT_NE(*resourceImage, *testImage)
      << "loaded resource image should not be equal to test image";
}

static void requestResources(
    ShovelerResources* resources, const char* typeId, const char* resourceId, void* testPointer) {
  ShovelerResourcesImagePngTest* test = (ShovelerResourcesImagePngTest*) testPointer;
  shovelerResourcesSet(
      resources,
      typeId,
      resourceId,
      reinterpret_cast<const unsigned char*>(test->testImageData.c_str()),
      test->testImageData.size());
}
