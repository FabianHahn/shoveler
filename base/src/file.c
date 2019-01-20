#include <errno.h> // errno
#include <stdio.h> // FILE, fopen, feof, fread, fclose
#include <stdlib.h> // NULL stdlib
#include <string.h> // strdup strerror

#include <glib.h>

#include "shoveler/file.h"
#include "shoveler/log.h"

#define READ_BUFFER_SIZE 4096

bool shovelerFileRead(const char *filename, unsigned char **contentsPointer, size_t *contentsSizePointer)
{
	FILE *file = fopen(filename, "rb");
	if(file == NULL) {
		shovelerLogError("Failed to read file from '%s': %s.", filename, strerror(errno));
		return false;
	}

	GString *contents = g_string_new("");
	unsigned char buffer[READ_BUFFER_SIZE];

	while(!feof(file)) {
		size_t numBytesRead = fread(buffer, 1, READ_BUFFER_SIZE, file);

		if(ferror(file)) {
			shovelerLogError("Error when read file from '%s' after %lu bytes: %s.", filename, contents->len, strerror(errno));
			g_string_free(contents, true);
			fclose(file);
			return false;
		}

		g_string_append_len(contents, (gchar *) buffer, numBytesRead);

		if(numBytesRead < READ_BUFFER_SIZE) {
			break;
		}
	}
	fclose(file);

	*contentsPointer = (unsigned char *) contents->str;
	*contentsSizePointer = contents->len;

	shovelerLogInfo("Successfully read %lu bytes from '%s'.", contents->len, filename);
	g_string_free(contents, false);
	return true;
}

char *shovelerFileReadString(const char *filename)
{
	unsigned char *contents;
	size_t contentsSize;
	if(!shovelerFileRead(filename, &contents, &contentsSize)) {
		return NULL;
	}

	return (char *) contents;
}

bool shovelerFileWrite(const char *filename, unsigned char *contents, size_t contentsSize)
{
	FILE *file = fopen(filename, "wb+");
	if(file == NULL) {
		shovelerLogError("Failed to write file to '%s': %s.", filename, strerror(errno));
		return false;
	}

	size_t contentsWritten = fwrite(contents, contentsSize, 1, file);
	if(contentsWritten != 1 || ferror(file)) {
		shovelerLogError("Error when writing %zu bytes to '%s': %s.", contentsSize, filename, strerror(errno));
		fclose(file);
		return false;
	}

	fclose(file);
	shovelerLogInfo("Successfully wrote %zu bytes to '%s'.", contentsSize, filename);
	return true;
}

bool shovelerFileWriteString(const char *filename, const char *string)
{
	return shovelerFileWrite(filename, (unsigned char *) string, strlen(string));
}
