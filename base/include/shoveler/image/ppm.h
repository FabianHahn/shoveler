#ifndef SHOVELER_IMAGE_PPM_H
#define SHOVELER_IMAGE_PPM_H

#include <shoveler/image.h>
#include <stdbool.h> // bool

ShovelerImage* shovelerImagePpmReadFile(const char* filename);
ShovelerImage* shovelerImagePpmReadBuffer(const unsigned char* buffer, int bufferSize);
bool shovelerImagePpmWriteFile(ShovelerImage* image, const char* filename);

#endif
