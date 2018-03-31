#ifndef FAKEGLIB_G_TIME_H
#define FAKEGLIB_G_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fakeglib/api.h>
#include <fakeglib/GTypes.h>

#define G_USEC_PER_SEC 1000000

FAKEGLIB_API gint64 g_get_monotonic_time();
FAKEGLIB_API gint64 g_get_real_time();

#ifdef __cplusplus
}
#endif

#endif
