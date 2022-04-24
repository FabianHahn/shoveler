#include "shoveler/compression.h"

#include <glib.h>
#include <stdlib.h> // NULL malloc free
#include <string.h> // memcpy
#include <zlib.h>

#include "shoveler/log.h"

static const char* getFormatName(ShovelerCompressionFormat format);
static int getWindowBitsForFormat(ShovelerCompressionFormat format);

static int inflateChunkSize = 512000;

bool shovelerCompressionCompress(
    ShovelerCompressionFormat format,
    const unsigned char* input,
    size_t inputSize,
    unsigned char** outputPointer,
    size_t* outputSizePointer) {
  z_stream stream;
  stream.avail_in = 0;
  stream.next_in = Z_NULL;
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;

  int ret = deflateInit2(
      &stream,
      Z_DEFAULT_COMPRESSION,
      Z_DEFLATED,
      getWindowBitsForFormat(format),
      8,
      Z_DEFAULT_STRATEGY);
  if (ret != Z_OK) {
    shovelerLogError(
        "Failed to initialize deflate when trying to compress buffer (%zu bytes) using %s: %s",
        inputSize,
        getFormatName(format),
        zError(ret));
    return false;
  }

  stream.avail_in = inputSize;
  stream.next_in = (Bytef*) input;

  uLong outputBufferSize = deflateBound(&stream, inputSize);
  unsigned char* outputBuffer = malloc(outputBufferSize * sizeof(unsigned char));
  stream.avail_out = outputBufferSize;
  stream.next_out = outputBuffer;

  ret = deflate(&stream, Z_FINISH);
  if (ret != Z_STREAM_END) {
    deflateEnd(&stream);
    shovelerLogError(
        "Failed to compress buffer (%zu bytes) using %s: %s",
        inputSize,
        getFormatName(format),
        zError(ret));
    return false;
  }

  *outputSizePointer = outputBufferSize - stream.avail_out;
  *outputPointer = malloc(*outputSizePointer * sizeof(unsigned char));
  memcpy(*outputPointer, outputBuffer, *outputSizePointer);
  free(outputBuffer);

  deflateEnd(&stream);

  shovelerLogInfo(
      "Successfully compressed buffer from %zu bytes to %zu bytes using %s.",
      inputSize,
      *outputSizePointer,
      getFormatName(format));
  return true;
}

bool shovelerCompressionDecompress(
    ShovelerCompressionFormat format,
    const unsigned char* input,
    size_t inputSize,
    unsigned char** outputPointer,
    size_t* outputSizePointer) {
  z_stream stream;
  stream.avail_in = 0;
  stream.next_in = Z_NULL;
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;

  int ret = inflateInit2(&stream, getWindowBitsForFormat(format));
  if (ret != Z_OK) {
    shovelerLogError(
        "Failed to initialize inflate when trying to decompress buffer (%zu bytes) using %s: %s",
        inputSize,
        getFormatName(format),
        zError(ret));
    return false;
  }

  stream.avail_in = inputSize;
  stream.next_in = (Bytef*) input;

  unsigned char* outputBuffer = malloc(inflateChunkSize * sizeof(unsigned char));

  GString* outputString = g_string_new("");

  do {
    stream.avail_out = inflateChunkSize;
    stream.next_out = outputBuffer;

    ret = inflate(&stream, Z_NO_FLUSH);
    if (ret != Z_OK && ret != Z_STREAM_END) {
      free(outputBuffer);
      g_string_free(outputString, true);
      inflateEnd(&stream);
      shovelerLogError(
          "Failed to decompress buffer (%zu bytes) using %s: %s",
          inputSize,
          getFormatName(format),
          zError(ret));
      return false;
    }

    uLong bytesRead = inflateChunkSize - stream.avail_out;
    g_string_append_len(outputString, (const char*) outputBuffer, bytesRead);
  } while (ret != Z_STREAM_END);

  free(outputBuffer);
  inflateEnd(&stream);

  *outputSizePointer = outputString->len;
  *outputPointer = malloc(*outputSizePointer * sizeof(unsigned char));
  memcpy(*outputPointer, outputString->str, *outputSizePointer);
  g_string_free(outputString, true);

  shovelerLogInfo(
      "Successfully decompressed buffer from %zu bytes to %zu bytes using %s.",
      inputSize,
      *outputSizePointer,
      getFormatName(format));
  return true;
}

static const char* getFormatName(ShovelerCompressionFormat format) {
  switch (format) {
  case SHOVELER_COMPRESSION_FORMAT_DEFLATE:
    return "deflate";
  case SHOVELER_COMPRESSION_FORMAT_ZLIB:
    return "zlib";
  case SHOVELER_COMPRESSION_FORMAT_GZIP:
    return "gzip";
  default:
    shovelerLogError("Encountered unknown compression format: %d", format);
    return "unknown";
  }
}

static int getWindowBitsForFormat(ShovelerCompressionFormat format) {
  switch (format) {
  case SHOVELER_COMPRESSION_FORMAT_DEFLATE:
    return -MAX_WBITS;
  case SHOVELER_COMPRESSION_FORMAT_ZLIB:
    return MAX_WBITS;
  case SHOVELER_COMPRESSION_FORMAT_GZIP:
    return MAX_WBITS | 16;
  default:
    shovelerLogError("Encountered unknown compression format: %d", format);
    return 0;
  }
}
