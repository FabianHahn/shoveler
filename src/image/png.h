#ifndef SHOVELER_IMAGE_PNG_H
#define SHOVELER_IMAGE_PNG_H

#include <stdbool.h> // bool

#include "image.h"

ShovelerImage *shovelerImagePngRead(const char *filename);
bool shovelerImagePngWrite(ShovelerImage *image, const char *filename);

#endif
