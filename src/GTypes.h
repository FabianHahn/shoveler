#ifndef FAKEGLIB_G_TYPES_H
#define FAKEGLIB_G_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h> // bool
#include <stddef.h> // size_t
#include <stdint.h> // int64

typedef char gchar;
typedef unsigned long gsize;
typedef signed long gssize;
typedef int gint;
typedef unsigned int guint;
typedef int64_t gint64;
typedef void * gpointer;
typedef const void * gconstpointer;
typedef bool gboolean;
typedef void (*GDestroyNotify)(gpointer data);
typedef gpointer (*GCopyFunc)(gconstpointer src, gpointer data);

#ifdef __cplusplus
}
#endif

#endif