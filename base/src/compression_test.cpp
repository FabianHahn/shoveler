#include <cstring> // strlen memcmp
#include <cstdlib> // free
#include <map>
#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/compression.h"
}

TEST(compression, compressAndDecompress)
{
	const char *testInput = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
		"eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
		"quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis "
		"aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat "
		"nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui "
		"officia deserunt mollit anim id est laborum.";
	size_t testInputSize = strlen(testInput);

	std::map<std::string, ShovelerCompressionFormat> testCases = {
		{"deflate", SHOVELER_COMPRESSION_FORMAT_DEFLATE},
		{"zlib", SHOVELER_COMPRESSION_FORMAT_ZLIB},
		{"gzip", SHOVELER_COMPRESSION_FORMAT_GZIP}};

	for(const auto& testCase : testCases) {
		const std::string& testCaseName = testCase.first;
		ShovelerCompressionFormat testCaseFormat = testCase.second;

		unsigned char *testOutput;
		size_t testOutputSize;
		bool compressed = shovelerCompressionCompress(testCaseFormat, (const unsigned char *) testInput, testInputSize, &testOutput, &testOutputSize);
		ASSERT_TRUE(compressed) << testCaseName << " compression should succeed";

		unsigned char *testReconstructedInput;
		size_t testReconstructedInputSize;
		bool decompressed = shovelerCompressionDecompress(testCaseFormat, testOutput, testOutputSize, &testReconstructedInput, &testReconstructedInputSize);
		ASSERT_TRUE(decompressed) << testCaseName << " decompression should succeed";
		ASSERT_EQ(testReconstructedInputSize, testInputSize) << testCaseName << " reconstructed input size should match input size";

		bool equal = memcmp(testInput, testReconstructedInput, testInputSize) == 0;
		ASSERT_TRUE(equal) << testCaseName << " reconstructed input matches input";

		free(testOutput);
		free(testReconstructedInput);
	}
}

TEST(compression, decompressInvalid)
{
	const char *testInput = "the bird is the word";
	size_t testInputSize = strlen(testInput);

	std::map<std::string, ShovelerCompressionFormat> testCases = {
		{"deflate", SHOVELER_COMPRESSION_FORMAT_DEFLATE},
		{"zlib", SHOVELER_COMPRESSION_FORMAT_ZLIB},
		{"gzip", SHOVELER_COMPRESSION_FORMAT_GZIP}};

	for(const auto& testCase : testCases) {
		const std::string& testCaseName = testCase.first;
		ShovelerCompressionFormat testCaseFormat = testCase.second;

		unsigned char *testOutput;
		size_t testOutputSize;
		bool compressed = shovelerCompressionDecompress(testCaseFormat, (const unsigned char *) testInput, testInputSize, &testOutput, &testOutputSize);
		ASSERT_FALSE(compressed) << testCaseName << " compression should fail";
	}
}
