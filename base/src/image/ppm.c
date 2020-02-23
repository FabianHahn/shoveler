#include <limits.h> // UCHAR_MAX
#include <stddef.h> // NULL
#include <stdio.h> // FILE, fopen, fgets, getc, ungetc, fscanf, fclose
#include <string.h> // strncmp

#include "shoveler/image/ppm.h"
#include "shoveler/input_stream.h"
#include "shoveler/log.h"

#define PPM_HEADER_BYTES 3

static const char *ppmHeader = "P3\n";

static ShovelerImage *readPpm(ShovelerInputStream *inputStream);

ShovelerImage *shovelerImagePpmReadFile(const char *filename)
{
	ShovelerInputStream *inputStream = shovelerInputStreamCreateFile(filename);
	if(inputStream == NULL) {
		return NULL;
	}

	ShovelerImage *image = readPpm(inputStream);

	shovelerInputStreamFree(inputStream);

	return image;
}

ShovelerImage *shovelerImagePpmReadBuffer(const unsigned char *buffer, int bufferSize)
{
	ShovelerInputStream *inputStream = shovelerInputStreamCreateMemory(buffer, bufferSize);
	if(inputStream == NULL) {
		return NULL;
	}

	ShovelerImage *image = readPpm(inputStream);

	shovelerInputStreamFree(inputStream);

	return image;
}

bool shovelerImagePpmWriteFile(ShovelerImage *image, const char *filename)
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

	fprintf(file, "P3\n# PPM image exported by shoveler\n%u %u\n%d\n", image->width, image->height, UCHAR_MAX);

	for(unsigned int y = 0; y < image->height; y++) {
		for(unsigned int x = 0; x < image->width; x++) {
			unsigned char r = shovelerImageGet(image, x, y, 0);
			unsigned char g = shovelerImageGet(image, x, y, 1);
			unsigned char b = shovelerImageGet(image, x, y, 2);
			fprintf(file, "%d %d %d\n", r, g, b);
		}
	}

	fclose(file);

	shovelerLogInfo("Successfully wrote PPM image of size (%u, %u) to '%s'.", image->width, image->height, filename);
	return true;
}

static ShovelerImage *readPpm(ShovelerInputStream *inputStream)
{
	// read magic number
	unsigned char header[3];
	if(shovelerInputStreamRead(inputStream, header, PPM_HEADER_BYTES) < PPM_HEADER_BYTES) {
		shovelerLogError("Failed to read PPM image from %s: failed to read PPM header.", shovelerInputStreamGetDescription(inputStream));
		return NULL;
	}

	if(strncmp((const char *) header, ppmHeader, PPM_HEADER_BYTES) != 0) {
		shovelerLogError("Failed to read PPM image from %s: invalid magic number.", shovelerInputStreamGetDescription(inputStream));
		return NULL;
	}

	// skip comments
	int c = shovelerInputStreamGetc(inputStream);
	while(c == '#') {
		// go to end of line
		while(c != '\n') {
			if(c == EOF) {
				shovelerLogError("Failed to read PPM image from %s: unexpected end of file when parsing header comment.", shovelerInputStreamGetDescription(inputStream));
				return NULL;
			}
			c = shovelerInputStreamGetc(inputStream);
		}
		c = shovelerInputStreamGetc(inputStream);
	}
	shovelerInputStreamUngetc(inputStream, c);

	// read header
	int width = shovelerInputStreamReadInt(inputStream);
	int height = shovelerInputStreamReadInt(inputStream);
	int maxValue = shovelerInputStreamReadInt(inputStream);
	if(width < 0 || height < 0 || maxValue <= 0) {
		shovelerLogError("Failed to read PPM image from %s: invalid header defining image of size (%d, %d) and max value of %d.", shovelerInputStreamGetDescription(inputStream), width, height, maxValue);
		return NULL;
	}

	ShovelerImage *image = shovelerImageCreate(width, height, 3);

	for(unsigned int y = 0; y < height; y++) {
		for(unsigned int x = 0; x < width; x++) {
			int r = shovelerInputStreamReadInt(inputStream);
			int g = shovelerInputStreamReadInt(inputStream);
			int b = shovelerInputStreamReadInt(inputStream);
			if(r < 0 || r > maxValue || g < 0 || g > maxValue || b < 0 || b > maxValue) {
				shovelerLogError("Failed to read PPM image from %s: invalid pixel (%d, %d) value of (%d, %d, %d).", shovelerInputStreamGetDescription(inputStream), x, y, r, g, b);
				shovelerImageFree(image);
				return NULL;
			}
			shovelerImageGet(image, x, y, 0) = (unsigned char) ((double) UCHAR_MAX * r / maxValue);
			shovelerImageGet(image, x, y, 1) = (unsigned char) ((double) UCHAR_MAX * g / maxValue);
			shovelerImageGet(image, x, y, 2) = (unsigned char) ((double) UCHAR_MAX * b / maxValue);

		}
	}

	shovelerLogInfo("Successfully read PPM image of size (%d, %d) from %s.", width, height, shovelerInputStreamGetDescription(inputStream));
	return image;
}
