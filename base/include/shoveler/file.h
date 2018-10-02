#ifndef SHOVELER_FILE_H
#define SHOVELER_FILE_H

bool shovelerFileRead(const char *filename, unsigned char **contentsPointer, size_t *contentsSizePointer);
char *shovelerFileReadString(const char *filename);

#endif
