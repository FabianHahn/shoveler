#include <errno.h> // errno
#include <stdio.h> // FILE, fopen, feof, fgets, fclose
#include <stdlib.h> // NULL
#include <string.h> // strdup strerror

#include <glib.h>

#include "shoveler/file.h"
#include "shoveler/log.h"

#define READ_BUFFER_SIZE 1024

char *shovelerFileRead(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if(file == NULL) {
		shovelerLogError("Failed to read file from '%s': %s.", filename, strerror(errno));
		return NULL;
	}

	GString *contents = g_string_new("");
	char buffer[READ_BUFFER_SIZE];

	while(!feof(file)) {
		if(fgets(buffer, READ_BUFFER_SIZE, file) != NULL) {
			g_string_append(contents, buffer);
		}
	}
	fclose(file);

	char *contentsStr = strdup(contents->str);
	g_string_free(contents, true);
	return contentsStr;
}
