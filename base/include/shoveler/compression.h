#ifndef SHOVELER_COMPRESSION_H
#define SHOVELER_COMPRESSION_H

#include <stdbool.h> // bool
#include <stddef.h> // size_t

typedef enum {
  SHOVELER_COMPRESSION_FORMAT_DEFLATE,
  SHOVELER_COMPRESSION_FORMAT_ZLIB,
  SHOVELER_COMPRESSION_FORMAT_GZIP
} ShovelerCompressionFormat;

bool shovelerCompressionCompress(
    ShovelerCompressionFormat format,
    const unsigned char* input,
    size_t inputSize,
    unsigned char** outputPointer,
    size_t* outputSizePointer);
bool shovelerCompressionDecompress(
    ShovelerCompressionFormat format,
    const unsigned char* input,
    size_t inputSize,
    unsigned char** outputPointer,
    size_t* outputSizePointer);

#endif
