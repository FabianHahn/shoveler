#ifndef FAKEGLIB_G_STRING_H
#define FAKEGLIB_G_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h> // va_list

#include <fakeglib/api.h>
#include <fakeglib/GTypes.h>

struct GStringStruct {
	gchar *str;
	gsize len;
	gsize allocated_len;
};
typedef struct GStringStruct GString;

FAKEGLIB_API GString *g_string_new(const gchar *init);
FAKEGLIB_API GString *g_string_new_len(const gchar *init, gssize len);
FAKEGLIB_API GString *g_string_sized_new(gsize dfl_size);
FAKEGLIB_API GString *g_string_assign(GString *string, const gchar *rval);
FAKEGLIB_API void g_string_vprintf(GString *string, const gchar *format, va_list args);
FAKEGLIB_API void g_string_append_vprintf(GString *string, const gchar *format, va_list args);
FAKEGLIB_API void g_string_printf(GString *string, const gchar *format, ...);
FAKEGLIB_API void g_string_append_printf(GString *string, const gchar *format, ...);
FAKEGLIB_API GString *g_string_append(GString *string, const gchar *val);
FAKEGLIB_API GString *g_string_append_c(GString *string, gchar c);
FAKEGLIB_API GString *g_string_append_len(GString *string, const gchar *val, gssize len);
FAKEGLIB_API GString *g_string_truncate(GString *string, gsize len);
FAKEGLIB_API GString *g_string_set_size(GString *string, gsize len);
FAKEGLIB_API gchar *g_string_free(GString *string, gboolean free_segment);

#ifdef __cplusplus
}
#endif

#endif
