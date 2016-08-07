#include <cassert> // assert
#include <climits> // LONG_MAX
#include <cstdarg> // va_list
#include <cstddef> // NULL
#include <cstdio> // vsnprintf
#include <cstdlib> // malloc, free
#include <cstring> // strdup, strlen, memcpy, memmove

#include "GString.h"

static void expandStringTo(GString *string, gsize len)
{
	if(len >= string->allocated_len) {
		gsize newAllocatedLen = 2 * len; // overallocate to amortize cost of repeated calls
		string->str = (gchar *) realloc(string->str, newAllocatedLen * sizeof(gchar));
		string->allocated_len = newAllocatedLen;
	}
}

FAKEGLIB_API GString *g_string_new(const gchar *init)
{
	GString *string = new GString{};
	string->str = strdup(init == NULL ? "" : init);
	string->len = (gsize) strlen(string->str);
	string->allocated_len = string->len + 1;
	return string;
}

FAKEGLIB_API GString *g_string_new_len(const gchar *init, gssize len)
{
	assert(len >= 0);
	assert(len <= LONG_MAX);

	GString *string = new GString{};
	string->len = (gsize) len;
	string->allocated_len = string->len + 1;
	string->str = (gchar *) malloc(string->allocated_len * sizeof(gchar));
	memcpy(string->str, init, len);
	string->str[string->len] = '\0';
	return string;
}

FAKEGLIB_API GString *g_string_sized_new(gsize len)
{
	assert(len >= 0);
	assert(len <= LONG_MAX);

	GString *string = new GString{};
	string->allocated_len = len + 1;
	string->str = (gchar *) malloc(string->allocated_len * sizeof(gchar));
	string->str[0] = '\0';
	string->len = 0;
	return string;
}

FAKEGLIB_API GString *g_string_assign(GString *string, const gchar *rval)
{
	size_t length = strlen(rval);
	assert(length <= LONG_MAX);

	if((gsize) length < string->allocated_len) {
		memmove(string->str, rval, length);
	} else {
		expandStringTo(string, (gsize) length);
		memcpy(string->str, rval, length);
	}
	string->len = (gsize) length;
	string->str[string->len] = '\0';
	return string;
}

FAKEGLIB_API void g_string_vprintf(GString *string, const gchar *format, va_list args)
{
	va_list copiedArgs;
	va_copy(copiedArgs, args);
	int ret = vsnprintf(string->str, string->allocated_len, format, copiedArgs);
	va_end(copiedArgs);
	if(ret < (int) string->allocated_len) {
		assert(ret >= 0);
		string->len = ret;
		return;
	}

	expandStringTo(string, (gsize) ret);
	string->len = ret;

	ret = vsnprintf(string->str, string->allocated_len, format, args);
	assert((gsize) ret < string->allocated_len);
}

FAKEGLIB_API void g_string_append_vprintf(GString *string, const gchar *format, va_list args)
{
	gsize remainingAllocatedLen = string->allocated_len - string->len;
	gchar *offsetStr = string->str + string->len;
	va_list copiedArgs;
	va_copy(copiedArgs, args);
	int ret = vsnprintf(offsetStr, remainingAllocatedLen, format, copiedArgs);
	va_end(copiedArgs);
	if(ret < (int) remainingAllocatedLen) {
		assert(ret >= 0);
		string->len += ret;
		return;
	}

	// not enough space, back off and reallocate
	expandStringTo(string, string->len + ret);
	offsetStr = string->str + string->len;
	int newRet = vsnprintf(offsetStr, ret + 1, format, args);
	assert(newRet == ret);
	string->len += ret;
}

FAKEGLIB_API void g_string_printf(GString *string, const gchar *format, ...)
{
	va_list args;
	va_start(args, format);
	g_string_vprintf(string, format, args);
	va_end(args);
}

FAKEGLIB_API void g_string_append_printf(GString *string, const gchar *format, ...)
{
	va_list args;
	va_start(args, format);
	g_string_append_vprintf(string, format, args);
	va_end(args);
}

FAKEGLIB_API GString *g_string_append(GString *string, const gchar *val)
{
	size_t length = strlen(val);
	assert(length <= LONG_MAX);
	gsize remainingAllocatedLen = string->allocated_len - string->len;

	if((gsize) length < remainingAllocatedLen) {
		gchar *offsetStr = string->str + string->len;
		memmove(offsetStr, val, length);
	} else {
		expandStringTo(string, string->len + (gsize) length);
		memcpy(string->str + string->len, val, length);
	}
	string->len = string->len + (gsize) length;
	string->str[string->len] = '\0';
	return string;
}

FAKEGLIB_API GString *g_string_append_c(GString *string, gchar c)
{
	gsize remainingAllocatedLen = string->allocated_len - string->len;

	if(remainingAllocatedLen == 1) {
		expandStringTo(string, string->len + 1);
	}
	string->str[string->len] = c;
	string->len++;
	string->str[string->len] = '\0';
	return string;
}

FAKEGLIB_API GString *g_string_append_len(GString *string, const gchar *val, gssize len)
{
	assert(len <= LONG_MAX);

	gsize remainingAllocatedLen = string->allocated_len - string->len;

	if((gsize) len < remainingAllocatedLen) {
		memmove(string->str + string->len, val, len);
	} else {
		expandStringTo(string, string->len + (gsize) len);
		memcpy(string->str + string->len, val, len);
	}
	string->len += (gsize) len;
	string->str[string->len] = '\0';
	return string;
}

FAKEGLIB_API GString *g_string_truncate(GString *string, gsize len)
{
	assert(len >= 0);

	if(len < string->len) {
		string->len = len;
		string->str[string->len] = '\0';
	}
	return string;
}

FAKEGLIB_API GString *g_string_set_size(GString *string, gsize len)
{
	assert(len >= 0);

	if(len >= string->len) {
		expandStringTo(string, len);
	}

	string->len = len;
	string->str[string->len] = '\0';
	return string;
}

FAKEGLIB_API gchar *g_string_free(GString *string, gboolean freeSegment)
{
	gchar *str;
	if(freeSegment) {
		str = NULL;
		free(string->str);
	} else {
		str = string->str;
	}
	delete string;

	return str;
}
