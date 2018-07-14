#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/image/png.h"
#include "shoveler/image.h"
}

static bool operator==(const ShovelerImage& a, const ShovelerImage& b) {
	return a.width == b.width
		&& a.height == b.height
		&& a.channels == b.channels
		&& memcmp(a.data, b.data, a.width * a.height * a.channels * sizeof(unsigned char)) == 0;
}

class ShovelerImagePngTest : public ::testing::Test {
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
		remove(testFilename);
	}

	const char *testFilename = "test.png";
	ShovelerImage *testImage;
};

TEST_F(ShovelerImagePngTest, writeReadFile)
{
	bool written = shovelerImagePngWriteFile(testImage, testFilename);
	ASSERT_TRUE(written) << "png should be written successfully";

	ShovelerImage *image = shovelerImagePngReadFile(testFilename);
	ASSERT_TRUE(image != NULL) << "png should be read successfully";
	ASSERT_EQ(*image, *testImage) << "read image should be equal to test image";
}

TEST_F(ShovelerImagePngTest, writeReadMemory)
{
	bool written = shovelerImagePngWriteFile(testImage, testFilename);
	ASSERT_TRUE(written) << "png should be written successfully";

	std::ifstream file(testFilename);
	std::stringstream stream;
	stream << file.rdbuf();
	std::string contents(stream.str());

	ShovelerImage *image = shovelerImagePngReadBuffer(reinterpret_cast<const unsigned char *>(contents.c_str()), contents.size());
	ASSERT_TRUE(image != NULL) << "png should be read successfully";
	ASSERT_EQ(*image, *testImage) << "read image should be equal to test image";
}
