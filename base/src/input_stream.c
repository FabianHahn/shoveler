#include "shoveler/input_stream.h"

#include <assert.h> // assert
#include <errno.h> // errno
#include <limits.h> // INT_MAX
#include <stdarg.h> // va_list va_start
#include <stdlib.h> // malloc free
#include <string.h> // strerror

#include "shoveler/log.h"

static int scanInputStream(ShovelerInputStream *inputStream, const char *extendedFormatString, void *output);

ShovelerInputStream *shovelerInputStreamCreateFile(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if(file == NULL) {
		shovelerLogError("Failed to open file '%s': %s.", filename, strerror(errno));
		return NULL;
	}

	ShovelerInputStream *inputStream = malloc(sizeof(ShovelerInputStream));
	inputStream->type = SHOVELER_INPUT_STREAM_TYPE_FILE;
	inputStream->fileSource.file = file;
	inputStream->fileSource.filename = filename;
	inputStream->description = g_string_new("");
	g_string_append_printf(inputStream->description, "file '%s'", filename);

	return inputStream;
}

ShovelerInputStream *shovelerInputStreamCreateMemory(const unsigned char *buffer, int size)
{
	ShovelerInputStream *inputStream = malloc(sizeof(ShovelerInputStream));
	inputStream->type = SHOVELER_INPUT_STREAM_TYPE_MEMORY;
	inputStream->memorySource.buffer = buffer;
	inputStream->memorySource.size = size;
	inputStream->memorySource.index = 0;
	inputStream->description = g_string_new("");
	g_string_append_printf(inputStream->description, "buffer (%d bytes)", size);

	return inputStream;
}

const char *shovelerInputStreamGetDescription(ShovelerInputStream *inputStream)
{
	return inputStream->description->str;
}

int shovelerInputStreamGetc(ShovelerInputStream *inputStream)
{
	if(inputStream->type == SHOVELER_INPUT_STREAM_TYPE_FILE) {
		return fgetc(inputStream->fileSource.file);
	} else {
		if(inputStream->memorySource.index >= inputStream->memorySource.size) {
			return EOF;
		}

		return inputStream->memorySource.buffer[inputStream->memorySource.index++];
	}
}

int shovelerInputStreamRead(ShovelerInputStream *inputStream, unsigned char *target, int size)
{
	assert(size > 0);

	if(inputStream->type == SHOVELER_INPUT_STREAM_TYPE_FILE) {
		size_t read = fread(target, /* byte size */ 1, (size_t) size, inputStream->fileSource.file);

		if(read > INT_MAX) {
			return 0;
		}

		return (int) read;
	} else {
		int bytesToRead = size;
		int bytesLeft = inputStream->memorySource.size - inputStream->memorySource.index;
		if(bytesToRead > bytesLeft) {
			bytesToRead = bytesLeft;
		}

		memcpy(target, inputStream->memorySource.buffer + inputStream->memorySource.index, (size_t) bytesToRead);
		inputStream->memorySource.index += bytesToRead;
		return bytesToRead;
	}
}

int shovelerInputStreamReadInt(ShovelerInputStream *inputStream)
{
	int value = 0;
	scanInputStream(inputStream, "%d%n", &value);
	return value;
}

int shovelerInputStreamUngetc(ShovelerInputStream *inputStream, int c)
{
	if(inputStream->type == SHOVELER_INPUT_STREAM_TYPE_FILE) {
		return ungetc(c, inputStream->fileSource.file);
	} else {
		if(inputStream->memorySource.index <= 0) {
			return EOF;
		}

		unsigned char previous = inputStream->memorySource.buffer[--inputStream->memorySource.index];
		if(previous != c) {
			return EOF;
		}

		return c;
	}
}

void shovelerInputStreamFree(ShovelerInputStream *inputStream)
{
	if(inputStream == NULL) {
		return;
	}

	if(inputStream->type == SHOVELER_INPUT_STREAM_TYPE_FILE) {
		fclose(inputStream->fileSource.file);
	}

	g_string_free(inputStream->description, /* freeSegment */ true);
	free(inputStream);
}

static int scanInputStream(ShovelerInputStream *inputStream, const char *extendedFormatString, void *output)
{
	int bytesRead;
	if(inputStream->type == SHOVELER_INPUT_STREAM_TYPE_FILE) {
		return fscanf(inputStream->fileSource.file, extendedFormatString, output, &bytesRead);
	} else {
		int ret = sscanf((const char *) inputStream->memorySource.buffer + inputStream->memorySource.index, extendedFormatString, output, &bytesRead);
		inputStream->memorySource.index += bytesRead;
		return ret;
	}
}
