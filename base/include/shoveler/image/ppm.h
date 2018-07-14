#ifndef SHOVELER_IMAGE_PPM_H
#define SHOVELER_IMAGE_PPM_H

#include <stdbool.h> // bool

#include <shoveler/image.h>

ShovelerImage *shovelerImagePpmRead(const char *filename);
bool shovelerImagePpmWrite(ShovelerImage *image, const char *filename);

#endif
