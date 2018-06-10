#ifndef FAKEGLIB_G_STRING_UTIL_H
#define FAKEGLIB_G_STRING_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fakeglib/api.h>
#include <fakeglib/GTypes.h>

FAKEGLIB_API gboolean g_str_has_prefix(const gchar *str, const gchar *prefix);
FAKEGLIB_API gboolean g_str_has_suffix(const gchar *str, const gchar *suffix);
FAKEGLIB_API gchar **g_strsplit(const gchar *string, const gchar *delimiter, gint max_tokens);
FAKEGLIB_API void g_strfreev(gchar **str_array);

#ifdef __cplusplus
}
#endif

#endif
