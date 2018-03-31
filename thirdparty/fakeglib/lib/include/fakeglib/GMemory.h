#ifndef FAKEGLIB_G_MEMORY_H
#define FAKEGLIB_G_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fakeglib/api.h>
#include <fakeglib/GTypes.h>

FAKEGLIB_API void g_free(gpointer mem);

#ifdef __cplusplus
}
#endif

#endif
