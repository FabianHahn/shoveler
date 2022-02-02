#ifndef FAKEGLIB_G_DATE_TIME_H
#define FAKEGLIB_G_DATE_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fakeglib/api.h>
#include <fakeglib/GTypes.h>

struct GDateTimeStruct;
typedef struct GDateTimeStruct GDateTime;

FAKEGLIB_API void g_date_time_unref(GDateTime *datetime);
FAKEGLIB_API GDateTime *g_date_time_ref(GDateTime *datetime);
FAKEGLIB_API GDateTime *g_date_time_new_now_local(void);
FAKEGLIB_API GDateTime *g_date_time_new_now_utc(void);
FAKEGLIB_API gint g_date_time_get_year(GDateTime *datetime);
FAKEGLIB_API gint g_date_time_get_month(GDateTime *datetime);
FAKEGLIB_API gint g_date_time_get_day_of_month(GDateTime *datetime);
FAKEGLIB_API gint g_date_time_get_hour(GDateTime *datetime);
FAKEGLIB_API gint g_date_time_get_minute(GDateTime *datetime);
FAKEGLIB_API gint g_date_time_get_second(GDateTime *datetime);
FAKEGLIB_API gint g_date_time_get_microsecond(GDateTime *datetime);
FAKEGLIB_API gint64 g_date_time_to_unix(GDateTime *datetime);

#ifdef __cplusplus
}
#endif

#endif
