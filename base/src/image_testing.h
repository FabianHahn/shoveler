#ifndef SHOVELER_IMAGE_TESTING_H
#define SHOVELER_IMAGE_TESTING_H

#include <iostream>

extern "C" {
#include <shoveler/image.h>
};

bool operator==(const ShovelerImage& a, const ShovelerImage& b);
bool operator!=(const ShovelerImage& a, const ShovelerImage& b);
std::ostream& operator<<(std::ostream& stream, const ShovelerImage& image);

#endif
