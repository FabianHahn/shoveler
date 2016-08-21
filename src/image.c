#include <stdlib.h> // malloc, free
#include <string.h> // memset

#include "image.h"

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

void shovelerImageFree(ShovelerImage *image)
{
	free(image->data);
	free(image);
}
