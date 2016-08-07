#ifndef FAKEGLIB_G_MEMORY_H
#define FAKEGLIB_G_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "api.h"
#include "GTypes.h"

FAKEGLIB_API void g_free(gpointer mem);

#ifdef __cplusplus
}
#endif

#endif