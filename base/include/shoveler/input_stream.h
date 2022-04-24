#ifndef SHOVELER_INPUT_STREAM_H
#define SHOVELER_INPUT_STREAM_H

#include <glib.h>
#include <stdio.h> // FILE

typedef enum {
  SHOVELER_INPUT_STREAM_TYPE_FILE,
  SHOVELER_INPUT_STREAM_TYPE_MEMORY,
} ShovelerInputStreamType;

typedef struct {
  ShovelerInputStreamType type;
  union {
    struct {
      FILE* file;
      const char* filename;
    } fileSource;
    struct {
      const unsigned char* buffer;
      int size;
      int index;
    } memorySource;
  };
  GString* description;
} ShovelerInputStream;

/**
 * Creates a file input stream, with the caller retaining ownership over the passed filename.
 *
 * The filename must remain valid until the input stream is freed.
 */
ShovelerInputStream* shovelerInputStreamCreateFile(const char* filename);
/**
 * Creates a memory input stream, with the caller retaining ownership over the passed buffer.
 *
 * The buffer must remain valid until the input stream is freed.
 */
ShovelerInputStream* shovelerInputStreamCreateMemory(const unsigned char* buffer, int size);
const char* shovelerInputStreamGetDescription(ShovelerInputStream* inputStream);
int shovelerInputStreamGetc(ShovelerInputStream* inputStream);
int shovelerInputStreamRead(ShovelerInputStream* inputStream, unsigned char* target, int size);
int shovelerInputStreamReadInt(ShovelerInputStream* inputStream);
int shovelerInputStreamUngetc(ShovelerInputStream* inputStream, int c);
void shovelerInputStreamFree(ShovelerInputStream* inputStream);

#endif
