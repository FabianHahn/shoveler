#ifndef SHOVELER_FILE_H
#define SHOVELER_FILE_H

#include <stdbool.h>
#include <stddef.h> // size_t

bool shovelerFileRead(
    const char* filename, unsigned char** contentsPointer, size_t* contentsSizePointer);
char* shovelerFileReadString(const char* filename);
bool shovelerFileWrite(const char* filename, unsigned char* contents, size_t contentsSize);
bool shovelerFileWriteString(const char* filename, const char* string);

#endif
