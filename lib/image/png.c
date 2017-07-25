#include <png.h>

#include <setjmp.h> // setjmp
#include <stdio.h> // fread
#include <stdlib.h> // malloc, free

#include "image/png.h"
#include "log.h"

ShovelerImage *shovelerImagePngRead(const char *filename)
{
	png_byte header[8];

	FILE *file = fopen(filename, "rb");
	if(file == NULL) {
		shovelerLogError("Failed to read PNG image from '%s': fopen failed.", filename);
		return NULL;
	}

	if(fread(header, 1, 8, file) <= 0 || png_sig_cmp(header, 0, 8)) {
		shovelerLogError("Failed to read PNG image from '%s': libpng header signature mismatch.", filename);
		fclose(file);
		return NULL;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		shovelerLogError("Failed to read PNG image from '%s': failed to create libpng read struct.", filename);
		fclose(file);
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(png_ptr == NULL) {
		shovelerLogError("Failed to read PNG image from '%s': failed to create libpng info struct.", filename);
		fclose(file);
		return false;
	}

	png_infop end_info_ptr = png_create_info_struct(png_ptr);
	if(png_ptr == NULL) {
		shovelerLogError("Failed to read PNG image from '%s': failed to create libpng end info struct.", filename);
		fclose(file);
		return false;
	}

	int height = 0;
	png_bytep *row_pointers = NULL;

	if(setjmp(png_jmpbuf(png_ptr))) {
		shovelerLogError("Failed to read PNG image from '%s': libpng called longjmp.", filename);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
		fclose(file);

		// Cleanup memory if already allocated
		if(row_pointers != NULL) {
			for(int y = 0; y < height; y++) {
				free(row_pointers[y]);
			}
			free(row_pointers);
		}

		return NULL;
	}

	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);
	int width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	if(bit_depth < 8) {
		png_set_packing(png_ptr); // make sure bit depths below 8 bit are expanded to a char
	} else if(bit_depth == 16) {
		png_set_scale_16(png_ptr); // make sure 16 bit gets truncated to 8 bit
	}

	int channels = 0;

	switch(color_type) {
		case PNG_COLOR_TYPE_GRAY:
			channels = 1;
		break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			channels = 2;
		break;
		case PNG_COLOR_TYPE_PALETTE:
			channels = 3;
			png_set_palette_to_rgb(png_ptr);
		break;
		case PNG_COLOR_TYPE_RGB:
			channels = 3;
		break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			channels = 4;
		break;
		default:
			shovelerLogError("Failed to read PNG image from '%s': unsupported color type.", filename);
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
			fclose(file);
			return NULL;
		break;
	}

	row_pointers = malloc(height * sizeof(png_bytep));
	for(int y = 0; y < height; y++) {
		row_pointers[y] = malloc(rowbytes * sizeof(png_byte));
	}

	png_read_image(png_ptr, row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

	fclose(file);

	ShovelerImage *image = shovelerImageCreate(width, height, channels);
	for(int y = 0; y < height; y++) {
		png_byte *row = row_pointers[y];
		for(int x = 0; x < width; x++) {
			unsigned char *ptr = &(row[x * channels]);
			for(int c = 0; c < channels; c++) {
				shovelerImageGet(image, x, y, c) = ptr[c];
			}
		}
	}

	// cleanup
	for(int y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);

	shovelerLogInfo("Successfully read %d-bit PNG image of size (%d, %d) with %d channels from '%s'.", bit_depth, width, height, channels, filename);
	return image;
}

bool shovelerImagePngWrite(ShovelerImage *image, const char *filename)
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
