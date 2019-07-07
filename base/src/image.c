#include <assert.h> // assert
#include <limits.h> // UINT_MAX
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset

#include "shoveler/image.h"

static unsigned int getBorderDistance(unsigned int width, unsigned int height, unsigned int i, unsigned int j);
static unsigned int minInt(unsigned int a, unsigned int b);

ShovelerImage *shovelerImageCreate(unsigned int width, unsigned int height, unsigned int channels)
{
	assert(width > 0);
	assert(height > 0);
	assert(channels > 0);
	assert(width <= UINT_MAX / height); // width * height won't overflow
	assert(height * width <= UINT_MAX / channels); // width * height * channels won't overflow

	ShovelerImage *image = malloc(sizeof(ShovelerImage));
	image->width = width;
	image->height = height;
	image->channels = channels;
	image->data = malloc(width * height * channels * sizeof(unsigned char));
	return image;
}

ShovelerImage *shovelerImageCreateCopy(ShovelerImage *input)
{
	ShovelerImage *image = shovelerImageCreate(input->width, input->height, input->channels);
	memcpy(image->data, input->data, input->width * input->height * input->channels * sizeof(unsigned char));
	return image;
}

ShovelerImage *shovelerImageCreateFlippedX(ShovelerImage *input)
{
	ShovelerImage *image = shovelerImageCreate(input->width, input->height, input->channels);

	for(unsigned int i = 0; i < image->width; i++) {
		for(unsigned int j = 0; j < image->height; j++) {
			for (unsigned int c = 0; c < image->channels; c++) {
				shovelerImageGet(image, i, j, c) = shovelerImageGet(input, image->width - i - 1, j, c);
			}
		}
	}

	return image;
}

ShovelerImage *shovelerImageCreateFlippedY(ShovelerImage *input)
{
	ShovelerImage *image = shovelerImageCreate(input->width, input->height, input->channels);

	for(unsigned int i = 0; i < image->width; i++) {
		for(unsigned int j = 0; j < image->height; j++) {
			for (unsigned int c = 0; c < image->channels; c++) {
				shovelerImageGet(image, i, j, c) = shovelerImageGet(input, i, image->height - j - 1, c);
			}
		}
	}

	return image;
}

ShovelerImage *shovelerImageCreateRotatedClockwise(ShovelerImage *input)
{
	ShovelerImage *image = shovelerImageCreate(input->height, input->width, input->channels);

	for(unsigned int i = 0; i < image->width; i++) {
		for(unsigned int j = 0; j < image->height; j++) {
			for (unsigned int c = 0; c < image->channels; c++) {
				shovelerImageGet(image, j, image->width - i - 1, c) = shovelerImageGet(input, i, j, c);
			}
		}
	}

	return image;
}

ShovelerImage *shovelerImageCreateRotatedCounterClockwise(ShovelerImage *input)
{
	ShovelerImage *image = shovelerImageCreate(input->height, input->width, input->channels);

	for(unsigned int i = 0; i < image->width; i++) {
		for(unsigned int j = 0; j < image->height; j++) {
			for (unsigned int c = 0; c < image->channels; c++) {
				shovelerImageGet(image, image->height - j - 1, i, c) = shovelerImageGet(input, i, j, c);
			}
		}
	}

	return image;
}

ShovelerImage *shovelerImageCreateAnimationTileset(ShovelerImage *input, int shiftAmount)
{
	assert(input->width == input->height);
	unsigned int size = input->width;

	ShovelerImage *moveImage = shovelerImageCreate(size, size, input->channels);
	shovelerImageClear(moveImage);
	shovelerImageAddSubImage(moveImage, shiftAmount, 0, input);
	ShovelerImage *moveImage2 = shovelerImageCreateFlippedX(moveImage);

	ShovelerImage *image = shovelerImageCreate(4 * size, 3 * size, input->channels);
	shovelerImageClear(image);
	shovelerImageAddSubImage(image, 0, 0, input);
	shovelerImageAddSubImage(image, 0, size, moveImage);
	shovelerImageAddSubImage(image, 0, 2 * size, moveImage2);

	ShovelerImage *downImage = shovelerImageCreateFlippedY(input);
	ShovelerImage *downMoveImage = shovelerImageCreateFlippedY(moveImage);
	ShovelerImage *downMoveImage2 = shovelerImageCreateFlippedY(moveImage2);
	shovelerImageAddSubImage(image, size, 0, downImage);
	shovelerImageAddSubImage(image, size, size, downMoveImage);
	shovelerImageAddSubImage(image, size, 2 * size, downMoveImage2);

	ShovelerImage *leftImage = shovelerImageCreateRotatedCounterClockwise(input);
	ShovelerImage *leftMoveImage = shovelerImageCreateRotatedCounterClockwise(moveImage);
	ShovelerImage *leftMoveImage2 = shovelerImageCreateRotatedCounterClockwise(moveImage2);
	shovelerImageAddSubImage(image, 2 * size, 0, leftImage);
	shovelerImageAddSubImage(image, 2 * size, size, leftMoveImage);
	shovelerImageAddSubImage(image, 2 * size, 2 * size, leftMoveImage2);

	ShovelerImage *rightImage = shovelerImageCreateRotatedClockwise(input);
	ShovelerImage *rightMoveImage = shovelerImageCreateRotatedClockwise(moveImage);
	ShovelerImage *rightMoveImage2 = shovelerImageCreateRotatedClockwise(moveImage2);
	shovelerImageAddSubImage(image, 3 * size, 0, rightImage);
	shovelerImageAddSubImage(image, 3 * size, size, rightMoveImage);
	shovelerImageAddSubImage(image, 3 * size, 2 * size, rightMoveImage2);

	shovelerImageFree(moveImage);
	shovelerImageFree(moveImage2);

	shovelerImageFree(downImage);
	shovelerImageFree(downMoveImage);
	shovelerImageFree(downMoveImage2);

	shovelerImageFree(leftImage);
	shovelerImageFree(leftMoveImage);
	shovelerImageFree(leftMoveImage2);

	shovelerImageFree(rightImage);
	shovelerImageFree(rightMoveImage);
	shovelerImageFree(rightMoveImage2);

	return image;
}

void shovelerImageClear(ShovelerImage *image)
{
	memset(image->data, 0, image->width * image->height * image->channels * sizeof(unsigned char));
}

void shovelerImageSet(ShovelerImage *image, ShovelerColor color, unsigned char alpha)
{
	for(unsigned int i = 0; i < image->width; i++) {
		for(unsigned int j = 0; j < image->height; j++) {
			for(unsigned int c = 0; c < image->channels; c++) {
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

void shovelerImageAddFrame(ShovelerImage *image, unsigned int size, ShovelerColor color)
{
	assert(image->channels == 4);

	for(unsigned int i = 0; i < image->width; i++) {
		for(unsigned int j = 0; j < image->height; j++) {
			unsigned int borderDistance = getBorderDistance(image->width, image->height, i, j);
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
	assert(xOffset < 0 || UINT_MAX - (unsigned int) xOffset >= subImage->width); // adding xOffset to subImage->width will never overflow
	assert(yOffset < 0 || UINT_MAX - (unsigned int) yOffset >= subImage->height); // adding yOffset to subImage->height will never overflow
	assert(image->channels == subImage->channels);

	for(unsigned int i = 0; i < subImage->width; i++) {
		if (xOffset < 0 && (unsigned int) -xOffset > i) {
			continue; // would result in negative imageI
		}

		unsigned int imageI = xOffset + i;
		if(imageI >= image->width) {
			continue;
		}

		for(unsigned int j = 0; j < subImage->height; j++) {
			if (yOffset < 0 && (unsigned int) -yOffset > j) {
				continue; // would result in negative imageJ
			}

			unsigned int imageJ = yOffset + j;
			if(imageJ >= image->height) {
				continue;
			}

			for(unsigned int c = 0; c < subImage->channels; c++) {
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

static unsigned int getBorderDistance(unsigned int width, unsigned int height, unsigned int i, unsigned int j)
{
	assert(i < width);
	assert(j < height);

	unsigned int xLow = i;
	unsigned int xHigh = width - 1 - i;
	unsigned int yLow = j;
	unsigned int yHigh = height - 1 - j;
	return minInt(xLow, minInt(xHigh, minInt(yLow, yHigh)));
}

static unsigned int minInt(unsigned int a, unsigned int b)
{
	if(a < b) {
		return a;
	} else {
		return b;
	}
}
