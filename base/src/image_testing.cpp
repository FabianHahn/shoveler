#include "image_testing.h"

#include <cstring> // memcmp

bool operator==(const ShovelerImage& a, const ShovelerImage& b) {
  return a.width == b.width && a.height == b.height && a.channels == b.channels &&
      memcmp(a.data, b.data, a.width * a.height * a.channels * sizeof(unsigned char)) == 0;
}

bool operator!=(const ShovelerImage& a, const ShovelerImage& b) { return !(a == b); }

std::ostream& operator<<(std::ostream& stream, const ShovelerImage& image) {
  stream << "image with dimensions (" << image.width << ", " << image.height << ") and "
         << image.channels << " channels:" << std::endl;

  for (int i = 0; i < image.width; i++) {
    for (int j = 0; j < image.height; j++) {
      stream << "\t(";
      for (int c = 0; c < image.channels; c++) {
        stream << (int) shovelerImageGet(&image, i, j, c);
        if (c != image.channels - 1) {
          stream << ", ";
        }
      }
      stream << ")" << std::endl;
    }
  }

  return stream;
}
