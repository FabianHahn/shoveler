#include <cassert> // assert
#include <cstddef> // NULL, ptrdiff_t, size_t
#include <cstdlib> // malloc, free
#include <cstring> // strncmp, strlen, strncpy
#include <iostream>

#include "fakeglib/GQueue.h"
#include "fakeglib/GStringUtil.h"

FAKEGLIB_API gboolean g_str_has_prefix(const gchar *string, const gchar *prefix)
{
	size_t stringLength = strlen(string);
	size_t prefixLength = strlen(prefix);

	if(prefixLength > stringLength) {
		return false;
	}

	return strncmp(string, prefix, prefixLength) == 0;
}

FAKEGLIB_API gboolean g_str_has_suffix(const gchar *string, const gchar *suffix)
{
	size_t stringLength = strlen(string);
	size_t suffixLength = strlen(suffix);

	if(suffixLength > stringLength) {
		return false;
	}

	return strncmp(string + (stringLength - suffixLength), suffix, suffixLength) == 0;
}

FAKEGLIB_API gchar **g_strsplit(const gchar *string, const gchar *delimiter, gint maxChunks)
{
	size_t stringLength = strlen(string);
	size_t delimiterLength = strlen(delimiter);
	assert(delimiterLength > 0);
	if(stringLength == 0) {
		gchar **result = (gchar **) malloc(sizeof(gchar *));
		result[0] = NULL;
		return result;
	}

	GQueue *resultQueue = g_queue_new();

	// loop over potential beginnings of chunks (iter)
	for(const gchar *iter = string; *iter != '\0'; ) {
		ptrdiff_t iterOffset = iter - string;
		assert(iterOffset >= 0);

		// loop over potential beginnings of matching delimiters (iter2)
		const gchar *iter2;
		for(iter2 = iter; *iter2 != '\0'; iter2++) {
			ptrdiff_t iter2Offset = iter2 - iter;
			assert(iter2Offset >= 0);

			size_t remaining = stringLength - iterOffset - iter2Offset;
			assert(remaining > 0);
			if(remaining < delimiterLength || (maxChunks > 0 && g_queue_get_length(resultQueue) == maxChunks - 1)) {
				// fast forward to the end
				iter2 = string + stringLength;
				assert(*iter2 == '\0');
				break;
			}

			if(strncmp(iter2, delimiter, delimiterLength) == 0) {
				gchar *chunk = (gchar *) malloc((iter2Offset + 1) * sizeof(gchar));
				strncpy(chunk, iter, (size_t) iter2Offset);
				chunk[iter2Offset] = '\0';
				g_queue_push_tail(resultQueue, chunk);

				iter = iter2 + delimiterLength;
				break;
			}
		}

		if(*iter2 == '\0') {
			// add last remaining chunk
			size_t remaining = stringLength - iterOffset;
			assert(remaining > 0);
			gchar *chunk = (gchar *) malloc((remaining + 1) * sizeof(gchar));
			strncpy(chunk, iter, (size_t) remaining);
			chunk[remaining] = '\0';
			g_queue_push_tail(resultQueue, chunk);
			break;
		}
	}

	guint numChunks = g_queue_get_length(resultQueue);
	gchar **result = (gchar **) malloc((numChunks + 1) * sizeof(gchar *));

	guint counter = 0;
	for(GList *iter = resultQueue->head; iter != NULL; iter = iter->next) {
		result[counter] = (gchar *) iter->data;
		counter++;
	}

	result[numChunks] = NULL;
	return result;
}

FAKEGLIB_API void g_strfreev(gchar **stringArray)
{
	if(stringArray == NULL) {
		return;
	}

	for(gchar **iter = stringArray; *iter != NULL; iter++) {
		free(*iter);
	}

	free(stringArray);
}
