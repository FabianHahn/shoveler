#ifndef FAKEGLIB_G_ARRAY_H
#define FAKEGLIB_G_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fakeglib/api.h>
#include <fakeglib/GTypes.h>

typedef struct GArrayStruct {
	gchar *data;
	guint len;
} GArray;

FAKEGLIB_API GArray *g_array_new(gboolean zero_terminated, gboolean clear_, guint element_size);
FAKEGLIB_API gpointer g_array_steal(GArray *array, gsize *len);
FAKEGLIB_API GArray *g_array_sized_new(gboolean zero_terminated, gboolean clear_, guint element_size, guint reserved_size);
FAKEGLIB_API GArray *g_array_copy(GArray *array);
FAKEGLIB_API GArray *g_array_ref(GArray *array);
FAKEGLIB_API void g_array_unref(GArray *array);
FAKEGLIB_API guint g_array_get_element_size(GArray *array);
#define g_array_append_val(a, v) g_array_append_vals(a, &(v), 1)
FAKEGLIB_API GArray *g_array_append_vals(GArray *array, gconstpointer data, guint len);
#define g_array_prepend_val(a, v) g_array_prepend_vals(a, &(v), 1)
FAKEGLIB_API GArray *g_array_prepend_vals(GArray *array, gconstpointer data, guint len);
#define g_array_insert_val(a, i, v) g_array_insert_vals(a, i, &(v), 1)
FAKEGLIB_API GArray *g_array_insert_vals(GArray *array, guint index_, gconstpointer data, guint len);
FAKEGLIB_API GArray *g_array_remove_index(GArray *array, guint index_);
FAKEGLIB_API GArray *g_array_remove_index_fast(GArray *array, guint index_);
FAKEGLIB_API GArray *g_array_remove_range(GArray *array, guint index_, guint length);
#define g_array_index(a, t, i) (((t*) (void *) (a)->data) [(i)])
FAKEGLIB_API GArray *g_array_set_size(GArray *array, guint length);
FAKEGLIB_API void g_array_set_clear_func(GArray *array, GDestroyNotify clear_func);
FAKEGLIB_API gchar *g_array_free(GArray *array, gboolean free_segment);

#ifdef __cplusplus
}
#endif

#endif
