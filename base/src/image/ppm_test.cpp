#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include "image_testing.h"

extern "C" {
#include "shoveler/image/ppm.h"
#include "shoveler/image.h"
}

class ShovelerImagePpmTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		remove(testFilename);

		testImage = shovelerImageCreate(2, 2, 3);
		shovelerImageClear(testImage);
		shovelerImageGet(testImage, 0, 0, 0) = 255;
		shovelerImageGet(testImage, 0, 1, 1) = 255;
		shovelerImageGet(testImage, 1, 0, 2) = 255;
		shovelerImageGet(testImage, 1, 1, 1) = 255;
		shovelerImageGet(testImage, 1, 1, 2) = 255;
	}

	virtual void TearDown()
	{
		shovelerImageFree(testImage);
		remove(testFilename);
	}

	const char *testFilename = "test.ppm";
	ShovelerImage *testImage;
};

TEST_F(ShovelerImagePpmTest, writeReadFile)
{
	bool written = shovelerImagePpmWriteFile(testImage, testFilename);
	ASSERT_TRUE(written) << "ppm should be written successfully";

	ShovelerImage *image = shovelerImagePpmReadFile(testFilename);
	ASSERT_TRUE(image != NULL) << "ppm should be read successfully";
	ASSERT_EQ(*image, *testImage) << "read image should be equal to test image";

	shovelerImageFree(image);
}

TEST_F(ShovelerImagePpmTest, writeReadMemory)
{
	bool written = shovelerImagePpmWriteFile(testImage, testFilename);
	ASSERT_TRUE(written) << "png should be written successfully";

	std::ifstream file(testFilename, std::ios::binary);
	std::stringstream stream;
	stream << file.rdbuf();
	std::string contents(stream.str());

	ShovelerImage *image = shovelerImagePpmReadBuffer(reinterpret_cast<const unsigned char *>(contents.c_str()), contents.size());
	ASSERT_TRUE(image != NULL) << "ppm should be read successfully";
	ASSERT_EQ(*image, *testImage) << "read image should be equal to test image";

	shovelerImageFree(image);
}
