#include "shoveler/image/png.h"

#include <assert.h> // assert
#include <glib.h>
#include <limits.h> // UINT_MAX
#include <png.h>
#include <setjmp.h> // setjmp
#include <stdio.h> // fread
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "shoveler/input_stream.h"
#include "shoveler/log.h"

#define PNG_HEADER_CHECK_BYTES 8

static ShovelerImage* readPng(ShovelerInputStream* inputStream);
static bool readPngData(
    png_structp png,
    png_infop info,
    png_infop endInfo,
    png_uint_32* width,
    volatile png_uint_32* height,
    png_uint_32* channels,
    png_byte* bitDepth,
    png_bytep* volatile* rowPointers);
static void readInputStream(png_structp png, png_bytep outBytes, png_size_t bytesToRead);
static ShovelerImage* createImageFromRowPointers(
    png_uint_32 width, png_uint_32 height, png_uint_32 channels, png_bytep* rowPointers);
static void handlePngReadError(png_structp png, png_const_charp error);
static void handlePngReadWarning(png_structp png, png_const_charp warning);
static void freeRowPointers(png_uint_32 height, png_bytep* rowPointers);

ShovelerImage* shovelerImagePngReadFile(const char* filename) {
  ShovelerInputStream* inputStream = shovelerInputStreamCreateFile(filename);
  if (inputStream == NULL) {
    return NULL;
  }

  ShovelerImage* image = readPng(inputStream);

  shovelerInputStreamFree(inputStream);

  return image;
}

ShovelerImage* shovelerImagePngReadBuffer(const unsigned char* buffer, int bufferSize) {
  ShovelerInputStream* inputStream = shovelerInputStreamCreateMemory(buffer, bufferSize);
  if (inputStream == NULL) {
    return NULL;
  }

  ShovelerImage* image = readPng(inputStream);

  shovelerInputStreamFree(inputStream);

  return image;
}

bool shovelerImagePngWriteFile(ShovelerImage* image, const char* filename) {
  assert(
      image->height <=
      UINT_MAX / sizeof(png_bytep)); // image->height * sizeof(png_bytep) won't overflow

  if (image->channels > 4) {
    shovelerLogError(
        "Failed to write PNG image to '%s': can only write image with up to 4 channels.", filename);
    return false;
  }

  FILE* file = fopen(filename, "wb+");
  if (file == NULL) {
    shovelerLogError("Failed to write PNG image to '%s': fopen failed.", filename);
    return false;
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    shovelerLogError(
        "Failed to write PNG image to '%s': failed to create libpng write struct.", filename);
    fclose(file);
    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    shovelerLogError(
        "Failed to write PNG image to '%s': failed to create libpng info struct.", filename);
    fclose(file);
    png_destroy_write_struct(&png_ptr, NULL);
    return false;
  }

  png_byte colorType = 0;
  switch (image->channels) {
  case 1:
    colorType = PNG_COLOR_TYPE_GRAY;
    break;
  case 2:
    colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
    break;
  case 3:
    colorType = PNG_COLOR_TYPE_RGB;
    break;
  case 4:
    colorType = PNG_COLOR_TYPE_RGB_ALPHA;
    break;
  default:
    shovelerLogError(
        "Failed to write PNG image to '%s': unsupported number of channels.", filename);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(file);
    return NULL;
    break;
  }

  png_bytep* row_pointers = malloc(image->height * sizeof(png_bytep));

  for (unsigned int y = 0; y < image->height; y++) {
    row_pointers[y] = malloc(image->width * image->channels * sizeof(png_byte));

    for (unsigned int x = 0; x < image->width; x++) {
      for (unsigned int c = 0; c < image->channels; c++) {
        row_pointers[y][image->channels * x + c] =
            shovelerImageGet(image, x, image->height - y - 1, c);
      }
    }
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    shovelerLogError("Failed to write PNG image to '%s': libpng called longjmp.", filename);
    fclose(file);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    // cleanup memory
    for (unsigned int y = 0; y < image->height; y++) {
      free(row_pointers[y]);
    }
    free(row_pointers);

    return false;
  }

  // set up writing
  png_init_io(png_ptr, file);

  // prepare header
  png_set_IHDR(
      png_ptr,
      info_ptr,
      image->width,
      image->height,
      8,
      colorType,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

  // prepare image contents
  png_set_rows(png_ptr, info_ptr, row_pointers);

  // write it to disk
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  // free the libpng context
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(file);

  // Cceanup memory
  for (unsigned int y = 0; y < image->height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  shovelerLogInfo(
      "Successfully wrote PNG image of size (%d, %d) with %d channels to '%s'.",
      image->width,
      image->height,
      image->channels,
      filename);
  return true;
}

static ShovelerImage* readPng(ShovelerInputStream* inputStream) {
  png_byte header[PNG_HEADER_CHECK_BYTES];
  if (shovelerInputStreamRead(inputStream, header, PNG_HEADER_CHECK_BYTES) <
      PNG_HEADER_CHECK_BYTES) {
    shovelerLogError(
        "Failed to read PNG image from %s: failed to read PNG header.",
        shovelerInputStreamGetDescription(inputStream));
    return NULL;
  }

  if (png_sig_cmp(header, 0, 8)) {
    shovelerLogError(
        "Failed to read PNG image from %s: libpng header signature mismatch.",
        shovelerInputStreamGetDescription(inputStream));
    return NULL;
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png == NULL) {
    shovelerLogError(
        "Failed to read PNG image from %s: failed to create libpng read struct.",
        shovelerInputStreamGetDescription(inputStream));
    return false;
  }

  png_infop info = png_create_info_struct(png);
  if (info == NULL) {
    shovelerLogError(
        "Failed to read PNG image from %s: failed to create libpng info struct.",
        shovelerInputStreamGetDescription(inputStream));
    return false;
  }

  png_infop endInfo = png_create_info_struct(png);
  if (endInfo == NULL) {
    shovelerLogError(
        "Failed to read PNG image from %s: failed to create libpng end info struct.",
        shovelerInputStreamGetDescription(inputStream));
    return false;
  }

  png_uint_32 width = 0;
  png_uint_32 channels = 0;
  png_byte bitDepth = 0;

  // the following variables are volatile because they are modified after setjmp, and accessed
  // within the setjmp
  volatile png_uint_32 height = 0;
  png_bytep* volatile rowPointers = NULL;

  if (setjmp(png_jmpbuf(png))) {
    png_destroy_read_struct(&png, &info, &endInfo);
    freeRowPointers(height, rowPointers);
    return NULL;
  }

  png_set_error_fn(png, inputStream, handlePngReadError, handlePngReadWarning);
  png_set_sig_bytes(png, 8);

  if (inputStream->type == SHOVELER_INPUT_STREAM_TYPE_FILE) {
    png_init_io(png, inputStream->fileSource.file);
  } else {
    png_set_read_fn(png, inputStream, readInputStream);
  }

  bool read = readPngData(png, info, endInfo, &width, &height, &channels, &bitDepth, &rowPointers);
  png_destroy_read_struct(&png, &info, &endInfo);
  if (!read) {
    return NULL;
  }

  ShovelerImage* image = createImageFromRowPointers(width, height, channels, rowPointers);
  freeRowPointers(height, rowPointers);

  shovelerLogInfo(
      "Successfully read %d-bit PNG image of size (%d, %d) with %d channels from %s.",
      bitDepth,
      width,
      height,
      channels,
      shovelerInputStreamGetDescription(inputStream));
  return image;
}

static bool readPngData(
    png_structp png,
    png_infop info,
    png_infop endInfo,
    png_uint_32* width,
    volatile png_uint_32* height,
    png_uint_32* channels,
    png_byte* bitDepth,
    png_bytep* volatile* rowPointersPointer) {
  png_read_info(png, info);
  *width = png_get_image_width(png, info);
  *height = png_get_image_height(png, info);
  *bitDepth = png_get_bit_depth(png, info);
  png_byte colorType = png_get_color_type(png, info);
  png_size_t rowbytes = png_get_rowbytes(png, info);

  if (*bitDepth < 8) {
    png_set_packing(png); // make sure bit depths below 8 bit are expanded to a char
  } else if (*bitDepth == 16) {
    png_set_scale_16(png); // make sure 16 bit gets truncated to 8 bit
  }

  *channels = 0;

  switch (colorType) {
  case PNG_COLOR_TYPE_GRAY:
    *channels = 1;
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA:
    *channels = 2;
    break;
  case PNG_COLOR_TYPE_PALETTE:
    *channels = 3;
    png_set_palette_to_rgb(png);
    break;
  case PNG_COLOR_TYPE_RGB:
    *channels = 3;
    break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
    *channels = 4;
    break;
  default:
    shovelerLogError("Failed to read PNG image: unsupported color type %d.", colorType);
    png_destroy_read_struct(&png, &info, &endInfo);
    return false;
    break;
  }

  *rowPointersPointer = malloc(*height * sizeof(png_bytep));
  for (png_uint_32 y = 0; y < *height; y++) {
    (*rowPointersPointer)[y] = malloc(rowbytes * sizeof(png_byte));
  }

  png_read_image(png, *rowPointersPointer);
  return true;
}

static void readInputStream(png_structp png, png_bytep outBytes, png_size_t bytesToReadSize) {
  if (bytesToReadSize > INT_MAX) {
    shovelerLogWarning(
        "Integer overflow when trying to read %zu bytes from memory stream.", bytesToReadSize);
    return;
  }
  int bytesToRead = (int) bytesToReadSize;

  ShovelerInputStream* inputStream = (ShovelerInputStream*) png_get_io_ptr(png);
  shovelerInputStreamRead(inputStream, outBytes, bytesToRead);
}

static ShovelerImage* createImageFromRowPointers(
    png_uint_32 width, png_uint_32 height, png_uint_32 channels, png_bytep* rowPointers) {
  ShovelerImage* image = shovelerImageCreate(width, height, channels);
  for (unsigned int y = 0; y < height; y++) {
    png_byte* row = rowPointers[y];
    for (unsigned int x = 0; x < width; x++) {
      unsigned char* ptr = &(row[x * channels]);
      for (unsigned int c = 0; c < channels; c++) {
        shovelerImageGet(image, x, height - y - 1, c) = ptr[c];
      }
    }
  }
  return image;
}

static void handlePngReadError(png_structp png, png_const_charp error) {
  ShovelerInputStream* inputStream = (ShovelerInputStream*) png_get_error_ptr(png);
  shovelerLogError(
      "Failed to read PNG image from %s due to libpng error: %s",
      shovelerInputStreamGetDescription(inputStream),
      error);
  png_longjmp(png, 1);
}

static void handlePngReadWarning(png_structp png, png_const_charp warning) {
  ShovelerInputStream* inputStream = (ShovelerInputStream*) png_get_error_ptr(png);
  shovelerLogWarning(
      "libpng warning while reading PNG image from %s: %s",
      shovelerInputStreamGetDescription(inputStream),
      warning);
}

static void freeRowPointers(png_uint_32 height, png_bytep* rowPointers) {
  // Cleanup memory if already allocated
  if (rowPointers != NULL) {
    for (png_uint_32 y = 0; y < height; y++) {
      free(rowPointers[y]);
    }
    free(rowPointers);
  }
}
