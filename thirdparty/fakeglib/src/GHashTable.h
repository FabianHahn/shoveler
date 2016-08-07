#ifndef FAKEGLIB_G_HASH_TABLE_H
#define FAKEGLIB_G_HASH_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "api.h"
#include "GTypes.h"
#include "GList.h"

typedef guint (*GHashFunc)(gconstpointer key);
typedef gboolean (*GEqualFunc)(gconstpointer a, gconstpointer b);
typedef void (*GHFunc)(gpointer key, gpointer value, gpointer user_data);
typedef gboolean (*GHRFunc)(gpointer key, gpointer value, gpointer user_data);

struct GHashTableStruct;
typedef struct GHashTableStruct GHashTable;

#ifndef FAKEGLIB_G_HASH_TABLE_ITER_SIZE
	#if _WIN32
		#define FAKEGLIB_G_HASH_TABLE_ITER_SIZE 40
	#else
		#define FAKEGLIB_G_HASH_TABLE_ITER_SIZE 32
	#endif
#endif
struct GHashTableIterStruct {
	unsigned char dummy[FAKEGLIB_G_HASH_TABLE_ITER_SIZE];
};
typedef struct GHashTableIterStruct GHashTableIter;

FAKEGLIB_API GHashTable *g_hash_table_new(GHashFunc hash_func, GEqualFunc key_equal_func);
FAKEGLIB_API GHashTable *g_hash_table_new_full(GHashFunc hash_func, GEqualFunc key_equal_func, GDestroyNotify key_destroy_func, GDestroyNotify value_destroy_func);
FAKEGLIB_API gboolean g_hash_table_insert(GHashTable *hash_table, gpointer key, gpointer value);
FAKEGLIB_API gboolean g_hash_table_replace(GHashTable *hash_table, gpointer key, gpointer value);
#define g_hash_table_add(hash_table, key) g_hash_table_replace(hash_table, key, key)
FAKEGLIB_API gboolean g_hash_table_contains(GHashTable *hash_table, gconstpointer key);
FAKEGLIB_API guint g_hash_table_size(GHashTable *hash_table);
FAKEGLIB_API gpointer g_hash_table_lookup(GHashTable *hash_table, gconstpointer key);
FAKEGLIB_API gboolean g_hash_table_lookup_extended(GHashTable *hash_table, gconstpointer lookup_key, gpointer *orig_key, gpointer *value);
FAKEGLIB_API void g_hash_table_foreach(GHashTable *hash_table, GHFunc func, gpointer user_data);
FAKEGLIB_API gpointer g_hash_table_find(GHashTable *hash_table, GHRFunc predicate, gpointer user_data);
FAKEGLIB_API gboolean g_hash_table_remove(GHashTable *hash_table, gconstpointer key);
FAKEGLIB_API gboolean g_hash_table_steal(GHashTable *hash_table, gconstpointer key);
FAKEGLIB_API guint g_hash_table_foreach_remove(GHashTable *hash_table, GHRFunc func, gpointer user_data);
FAKEGLIB_API guint g_hash_table_foreach_steal(GHashTable *hash_table, GHRFunc func, gpointer user_data);
FAKEGLIB_API void g_hash_table_remove_all(GHashTable *hash_table);
FAKEGLIB_API void g_hash_table_steal_all(GHashTable *hash_table);
FAKEGLIB_API GList *g_hash_table_get_keys(GHashTable *hash_table);
FAKEGLIB_API GList *g_hash_table_get_values(GHashTable *hash_table);
FAKEGLIB_API gpointer *g_hash_table_get_keys_as_array(GHashTable *hash_table, guint *length);
#define g_hash_table_freeze(hash_table)
#define g_hash_table_thaw(hash_table)
FAKEGLIB_API void g_hash_table_destroy(GHashTable *hash_table);
FAKEGLIB_API GHashTable *g_hash_table_ref(GHashTable *hash_table);
FAKEGLIB_API void g_hash_table_unref(GHashTable *hash_table);
FAKEGLIB_API void g_hash_table_iter_init(GHashTableIter *iter, GHashTable *hash_table);
FAKEGLIB_API GHashTable *g_hash_table_iter_get_hash_table(GHashTableIter *iter);
FAKEGLIB_API gboolean g_hash_table_iter_next(GHashTableIter *iter, gpointer *key, gpointer *value);
FAKEGLIB_API void g_hash_table_iter_replace(GHashTableIter *iter, gpointer value);
FAKEGLIB_API void g_hash_table_iter_remove(GHashTableIter *iter);
FAKEGLIB_API void g_hash_table_iter_steal(GHashTableIter *iter);
FAKEGLIB_API gboolean g_direct_equal(gconstpointer v1, gconstpointer v2);
FAKEGLIB_API guint g_direct_hash(gconstpointer v);
FAKEGLIB_API gboolean g_int_equal(gconstpointer v1, gconstpointer v2);
FAKEGLIB_API guint g_int_hash(gconstpointer v);
FAKEGLIB_API gboolean g_int64_equal(gconstpointer v1, gconstpointer v2);
FAKEGLIB_API guint g_int64_hash(gconstpointer v);
FAKEGLIB_API gboolean g_double_equal(gconstpointer v1, gconstpointer v2);
FAKEGLIB_API guint g_double_hash(gconstpointer v);
FAKEGLIB_API gboolean g_str_equal(gconstpointer v1, gconstpointer v2);
FAKEGLIB_API guint g_str_hash(gconstpointer v);

#ifdef __cplusplus
}
#endif

#endif