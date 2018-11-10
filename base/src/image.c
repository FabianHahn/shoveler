#include <assert.h> // assert
#include <stdlib.h> // malloc, free
#include <string.h> // memset

#include "shoveler/image.h"

static int getBorderDistance(int width, int height, int i, int j);
static int min(int a, int b);

ShovelerImage *shovelerImageCreate(int width, int height, int channels)
{
	ShovelerImage *image = malloc(sizeof(ShovelerImage));
	image->width = width;
	image->height = height;
	image->channels = channels;
	image->data = malloc(width * height * channels * sizeof(unsigned char));
	return image;
}

void shovelerImageClear(ShovelerImage *image)
{
	memset(image->data, 0, image->width * image->height * image->channels * sizeof(unsigned char));
}

void shovelerImageSet(ShovelerImage *image, ShovelerColor color, unsigned char alpha)
{
	for(int i = 0; i < image->width; i++) {
		for(int j = 0; j < image->height; j++) {
			for(int c = 0; c < image->channels; c++) {
				unsigned char value;
				if(c == 0) {
					value = color.r;
				} else if(c == 1) {
					value = color.g;
				} else if(c == 2) {
					value = color.b;
				} else {
					value = alpha;
				}
				shovelerImageGet(image, i, j, c) = value;
			}
		}
	}
}

void shovelerImageAddFrame(ShovelerImage *image, int size, ShovelerColor color)
{
	assert(image->channels == 4);

	for(int i = 0; i < image->width; i++) {
		for(int j = 0; j < image->height; j++) {
			int borderDistance = getBorderDistance(image->width, image->height, i, j);
			if(borderDistance < size) {
				unsigned char alpha = 255.0 * ((double) (size - borderDistance) / size);
				shovelerImageGet(image, i, j, 0) = color.r;
				shovelerImageGet(image, i, j, 1) = color.r;
				shovelerImageGet(image, i, j, 2) = color.r;
				shovelerImageGet(image, i, j, 3) = alpha;
			}
		}
	}
}

void shovelerImageAddSubImage(ShovelerImage *image, int xOffset, int yOffset, ShovelerImage *subImage)
{
	assert(image->channels == subImage->channels);

	for(int i = 0; i < subImage->width; i++) {
		int imageI = xOffset + i;
		if(imageI >= 0 && imageI < image->width) {
			continue;
		}

		for(int j = 0; j < subImage->height; j++) {
			int imageJ = yOffset + j;
			if(imageJ >= 0 && imageJ < image->height) {
				continue;
			}

			for(int c = 0; c < subImage->channels; c++) {
				shovelerImageGet(image, imageI, imageJ, c) = shovelerImageGet(subImage, i, j, c);
			}
		}
	}
}

void shovelerImageFree(ShovelerImage *image)
{
	if(image == NULL) {
		return;
	}

	free(image->data);
	free(image);
}

static int getBorderDistance(int width, int height, int i, int j)
{
	int xLow = i;
	int xHigh = width - 1 - i;
	int yLow = j;
	int yHigh = height - 1 - j;
	return min(xLow, min(xHigh, min(yLow, yHigh)));
}

static int min(int a, int b)
{
	if(a < b) {
		return a;
	} else {
		return b;
	}
}
