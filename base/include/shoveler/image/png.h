#ifndef SHOVELER_IMAGE_PNG_H
#define SHOVELER_IMAGE_PNG_H

#include <shoveler/image.h>
#include <stdbool.h> // bool
#include <stddef.h> // size_t

ShovelerImage* shovelerImagePngReadFile(const char* filename);
ShovelerImage* shovelerImagePngReadBuffer(const unsigned char* buffer, int bufferSize);
bool shovelerImagePngWriteFile(ShovelerImage* image, const char* filename);

#endif
