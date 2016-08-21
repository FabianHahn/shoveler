#ifndef SHOVELER_IMAGE_H
#define SHOVELER_IMAGE_H

typedef struct {
	int width;
	int height;
	int channels;
	unsigned char *data;
} ShovelerImage;

ShovelerImage *shovelerImageCreate(int width, int height, int channels);
void shovelerImageClear(ShovelerImage *image);
void shovelerImageFree(ShovelerImage *image);

#define shovelerImageGet(image, x, y, c) image->data[y * image->width * image->channels + x * image->channels + c]

#endif
