#include <limits.h> // UCHAR_MAX
#include <stddef.h> // NULL
#include <stdio.h> // FILE, fopen, fgets, getc, ungetc, fscanf, fclose
#include <string.h> // strncmp

#include "image/ppm.h"
#include "log.h"

ShovelerImage *shovelerImagePpmRead(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if(file == NULL) {
		shovelerLogError("Failed to read PPM image from '%s': fopen failed.", filename);
		return NULL;
	}

	// read magic number
	char magic[4];
	fgets(magic, 3 + 1, file);
	if(strncmp(magic, "P3\n", 3) != 0) {
		shovelerLogError("Failed to read PPM image from '%s': invalid magic number.", filename);
		fclose(file);
		return NULL;
	}

	// skip comments
	int c = getc(file);
	while(c == '#') {
		// go to end of line
		while(c != '\n') {
			if(c == EOF) {
				shovelerLogError("Failed to read PPM image from '%s': unexpected end of file when parsing header comment.", filename);
				fclose(file);
				return NULL;
			}
			c = getc(file);
		}
		c = getc(file);
	}
	ungetc(c, file);

	// read header
	int width = 0;
	int height = 0;
	fscanf(file, "%d %d", &width, &height);

	if(width <= 0) {
		shovelerLogError("Failed to read PPM image from '%s': parsed invalid width of %d.", filename, width);
		fclose(file);
		return NULL;
	}

	if(height <= 0) {
		shovelerLogError("Failed to read PPM image from '%s': parsed invalid height of %d.", filename, height);
		fclose(file);
		return NULL;
	}

	int maxValue = 0;
	fscanf(file, "%d", &maxValue);

	if(maxValue <= 0) {
		shovelerLogError("Failed to read PPM image from '%s': parsed invalid max value of %d.", filename, maxValue);
		fclose(file);
		return NULL;
	}

	ShovelerImage *image = shovelerImageCreate(width, height, 3);

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			int r = 0;
			int g = 0;
			int b = 0;
			fscanf(file, "%d", &r);
			fscanf(file, "%d", &g);
			fscanf(file, "%d", &b);
			if(r > maxValue || g > maxValue || b > maxValue) {
				shovelerLogError("Failed to read PPM image from '%s': invalid pixel (%d, %d) value of (%d, %d, %d).", filename, x, y, r, g, b);
				shovelerImageFree(image);
				fclose(file);
				return NULL;
			}
			shovelerImageGet(image, x, y, 0) = (unsigned char) ((double) UCHAR_MAX * r / maxValue);
			shovelerImageGet(image, x, y, 1) = (unsigned char) ((double) UCHAR_MAX * g / maxValue);
			shovelerImageGet(image, x, y, 2) = (unsigned char) ((double) UCHAR_MAX * b / maxValue);

		}
	}

	fclose(file);
	shovelerLogError("Successfully read PPM image of size (%d, %d) from '%s'.", width, height, filename);
	return image;
}

bool shovelerImagePpmWrite(ShovelerImage *image, const char *filename)
{
	if(image->channels < 3) {
		shovelerLogError("Failed to write PPM image to '%s': can only write image with at least 3 channels.", filename);
		return false;
	}

	FILE *file = fopen(filename, "w+");
	if(file == NULL) {
		shovelerLogError("Failed to write PPM image to '%s': fopen failed.", filename);
		return false;
	}

	fprintf(file, "P3\n# PPM image exported by shoveler\n%d %d\n%d\n", image->width, image->height, UCHAR_MAX);

	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char r = shovelerImageGet(image, x, y, 0);
			unsigned char g = shovelerImageGet(image, x, y, 1);
			unsigned char b = shovelerImageGet(image, x, y, 2);
			fprintf(file, "%d %d %d\n", r, g, b);
		}
	}

	fclose(file);
	shovelerLogError("Successfully wrote PPM image to '%s'.", filename);
	return true;
}
