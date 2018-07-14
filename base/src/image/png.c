#include <glib.h>
#include <png.h>

#include <setjmp.h> // setjmp
#include <stdio.h> // fread
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "shoveler/image/png.h"
#include "shoveler/log.h"

typedef struct {
	const unsigned char *buffer;
	size_t size;
	size_t index;
} MemoryStream;

typedef enum {
	READ_INPUT_FILE,
	READ_INPUT_MEMORY_STREAM,
} ReadInputType;

typedef union {
	FILE *file;
	MemoryStream memoryStream;
} ReadInputValue;

typedef struct {
	ReadInputType type;
	ReadInputValue value;
	char *description;
} ReadInput;

static ShovelerImage *readPng(ReadInput input);
static bool readPngData(png_structp png, png_infop info, png_infop endInfo, png_uint_32 *width, volatile png_uint_32 *height, png_uint_32 *channels, png_byte *bitDepth, png_bytep * volatile *rowPointers);
static void readMemoryStream(png_structp png, png_bytep outBytes, png_size_t bytesToRead);
static ShovelerImage *createImageFromRowPointers(png_uint_32 width, png_uint_32 height, png_uint_32 channels, png_bytep *rowPointers);
static void handlePngReadError(png_structp png, png_const_charp error);
static void handlePngReadWarning(png_structp png, png_const_charp warning);
static void freeRowPointers(png_uint_32 height, png_bytep *rowPointers);

ShovelerImage *shovelerImagePngReadFile(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if(file == NULL) {
		shovelerLogError("Failed to read PNG image from file '%s': fopen failed.", filename);
		return NULL;
	}

	GString *descriptionString = g_string_new("");
	g_string_append_printf(descriptionString, "file '%s'", filename);

	ReadInput readInput = {READ_INPUT_FILE, {file}, descriptionString->str};
	ShovelerImage *image = readPng(readInput);

	g_string_free(descriptionString, true);
	fclose(file);

	return image;
}

ShovelerImage *shovelerImagePngReadBuffer(const unsigned char *buffer, size_t size)
{
	GString *descriptionString = g_string_new("");
	g_string_append_printf(descriptionString, "buffer (%u bytes)", size);

	ReadInputValue readInputValue;
	readInputValue.memoryStream = (MemoryStream){buffer, size, 0};
	ReadInput readInput = {READ_INPUT_MEMORY_STREAM, readInputValue, descriptionString->str};
	ShovelerImage *image = readPng(readInput);

	g_string_free(descriptionString, true);

	return image;
}

bool shovelerImagePngWriteFile(ShovelerImage *image, const char *filename)
{
	if(image->channels > 4) {
		shovelerLogError("Failed to write PNG image to '%s': can only write image with up to 4 channels.", filename);
		return false;
	}

	FILE *file = fopen(filename, "wb+");
	if(file == NULL) {
		shovelerLogError("Failed to write PNG image to '%s': fopen failed.", filename);
		return false;
	}

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		shovelerLogError("Failed to write PNG image to '%s': failed to create libpng write struct.", filename);
		fclose(file);
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL) {
		shovelerLogError("Failed to write PNG image to '%s': failed to create libpng info struct.", filename);
		fclose(file);
		png_destroy_write_struct(&png_ptr, NULL);
		return false;
	}

	png_byte colorType = 0;
	switch(image->channels) {
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
			shovelerLogError("Failed to write PNG image to '%s': unsupported number of channels.", filename);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			fclose(file);
			return NULL;
		break;
	}

	png_bytep *row_pointers = malloc(image->height * sizeof(png_bytep));

	for(int y = 0; y < image->height; y++) {
		row_pointers[y] = malloc(image->width * image->channels * sizeof(png_byte));

		for(int x = 0; x < image->width; x++) {
			for(int c = 0; c < image->channels; c++) {
				row_pointers[y][image->channels * x + c] = shovelerImageGet(image, x, y, c);
			}
		}
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		shovelerLogError("Failed to write PNG image to '%s': libpng called longjmp.", filename);
		fclose(file);
		png_destroy_write_struct(&png_ptr, &info_ptr);

		// cleanup memory
		for(int y = 0; y < image->height; y++) {
			free(row_pointers[y]);
		}
		free(row_pointers);

		return false;
	}

	// set up writing
	png_init_io(png_ptr, file);

	// prepare header
	png_set_IHDR(png_ptr, info_ptr, image->width, image->height, 8, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	// prepare image contents
	png_set_rows(png_ptr, info_ptr, row_pointers);

	// write it to disk
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	// free the libpng context
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(file);

	// Cceanup memory
	for(int y = 0; y < image->height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);

	shovelerLogInfo("Successfully wrote PNG image of size (%d, %d) with %d channels to '%s'.", image->width, image->height, image->channels, filename);
	return true;
}

static ShovelerImage *readPng(ReadInput input)
{
	switch(input.type) {
		case READ_INPUT_FILE: {
			png_byte header[8];

			if(fread(header, 1, 8, input.value.file) <= 0 || png_sig_cmp(header, 0, 8)) {
				shovelerLogError("Failed to read PNG image from %s: libpng header signature mismatch.", input.description);
				return NULL;
			}
		} break;
		case READ_INPUT_MEMORY_STREAM:
			if(png_sig_cmp(input.value.memoryStream.buffer, 0, 8)) {
				shovelerLogError("Failed to read PNG image from %s: libpng header signature mismatch.", input.description);
				return NULL;
			}
		break;
		default:
			shovelerLogError("Failed to read PNG image from %s: invalid input type %d.", input.description, input.type);
		break;
	}


	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png == NULL) {
		shovelerLogError("Failed to read PNG image from %s: failed to create libpng read struct.", input.description);
		return false;
	}

	png_infop info = png_create_info_struct(png);
	if(info == NULL) {
		shovelerLogError("Failed to read PNG image from %s: failed to create libpng info struct.", input.description);
		return false;
	}

	png_infop endInfo = png_create_info_struct(png);
	if(endInfo == NULL) {
		shovelerLogError("Failed to read PNG image from %s: failed to create libpng end info struct.", input.description);
		return false;
	}

	png_uint_32 width = 0;
	png_uint_32 channels = 0;
	png_byte bitDepth = 0;

	// the following variables are volatile because they are modified after setjmp, and accessed within the setjmp
	volatile png_uint_32 height = 0;
	png_bytep * volatile rowPointers = NULL;

	if(setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, &info, &endInfo);
		freeRowPointers(height, rowPointers);
		return NULL;
	}

	png_set_error_fn(png, &input, handlePngReadError, handlePngReadWarning);

	switch(input.type) {
		case READ_INPUT_FILE:
			png_init_io(png, input.value.file);
			png_set_sig_bytes(png, 8);
		break;
		case READ_INPUT_MEMORY_STREAM:
			png_set_read_fn(png, &input.value.memoryStream, readMemoryStream);
			png_set_sig_bytes(png, 0);
		break;
		default:
			shovelerLogError("Failed to read PNG image from %s: invalid input type %d.", input.description, input.type);
		break;
	}

	bool read = readPngData(png, info, endInfo, &width, &height, &channels, &bitDepth, &rowPointers);
	png_destroy_read_struct(&png, &info, &endInfo);
	if(!read) {
		return NULL;
	}

	ShovelerImage *image = createImageFromRowPointers(width, height, channels, rowPointers);
	freeRowPointers(height, rowPointers);

	shovelerLogInfo("Successfully read %d-bit PNG image of size (%d, %d) with %d channels from %s.", bitDepth, width, height, channels, input.description);
	return image;
}

static bool readPngData(png_structp png, png_infop info, png_infop endInfo, png_uint_32 *width, volatile png_uint_32 *height, png_uint_32 *channels, png_byte *bitDepth, png_bytep * volatile *rowPointersPointer)
{
	png_read_info(png, info);
	*width = png_get_image_width(png, info);
	*height = png_get_image_height(png, info);
	*bitDepth = png_get_bit_depth(png, info);
	png_byte colorType = png_get_color_type(png, info);
	png_uint_32 rowbytes = png_get_rowbytes(png, info);

	if(*bitDepth < 8) {
		png_set_packing(png); // make sure bit depths below 8 bit are expanded to a char
	} else if(*bitDepth == 16) {
		png_set_scale_16(png); // make sure 16 bit gets truncated to 8 bit
	}

	*channels = 0;

	switch(colorType) {
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
	for(png_uint_32 y = 0; y < *height; y++) {
		(*rowPointersPointer)[y] = malloc(rowbytes * sizeof(png_byte));
	}

	png_read_image(png, *rowPointersPointer);
	return true;
}

static void readMemoryStream(png_structp png, png_bytep outBytes, png_size_t bytesToRead)
{
	MemoryStream *memoryStream = (MemoryStream *) png_get_io_ptr(png);

	size_t bytesLeft = memoryStream->size - memoryStream->index;
	if(bytesToRead > bytesLeft) {
		shovelerLogWarning("Tryed to read %u bytes from memory stream but only %u bytes are left - returning those.", bytesToRead, bytesLeft);
		bytesToRead = bytesLeft;
	}

	memcpy(outBytes, memoryStream->buffer + memoryStream->index, bytesToRead);
	memoryStream->index += bytesToRead;
}

static ShovelerImage *createImageFromRowPointers(png_uint_32 width, png_uint_32 height, png_uint_32 channels, png_bytep *rowPointers)
{
	ShovelerImage *image = shovelerImageCreate(width, height, channels);
	for(int y = 0; y < height; y++) {
		png_byte *row = rowPointers[y];
		for(int x = 0; x < width; x++) {
			unsigned char *ptr = &(row[x * channels]);
			for(int c = 0; c < channels; c++) {
				shovelerImageGet(image, x, y, c) = ptr[c];
			}
		}
	}
	return image;
}

static void handlePngReadError(png_structp png, png_const_charp error)
{
	ReadInput *readInput = (ReadInput *) png_get_error_ptr(png);
	shovelerLogError("Failed to read PNG image from %s due to libpng error: %s", readInput->description, error);
	png_longjmp(png, 1);
}

static void handlePngReadWarning(png_structp png, png_const_charp warning)
{
	ReadInput *readInput = (ReadInput *) png_get_error_ptr(png);
	shovelerLogWarning("libpng warning while reading PNG image from %s: %s", readInput->description, warning);
}

static void freeRowPointers(png_uint_32 height, png_bytep *rowPointers)
{
	// Cleanup memory if already allocated
	if(rowPointers != NULL) {
		for(png_uint_32 y = 0; y < height; y++) {
			free(rowPointers[y]);
		}
		free(rowPointers);
	}
}
