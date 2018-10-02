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
	FILE *file = fopen(filename, "r");
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
