#include <cstddef> // NULL
#include <ctime> // tm

#include <chrono>
#include <mutex>

#include "fakeglib/GDateTime.h"

static std::mutex chronoMutex;
static std::mutex referenceMutex;

struct GDateTimeStruct {
	std::chrono::system_clock::time_point time;
	std::time_t timeT;
	bool isUtc;
	unsigned int referenceCount;
};

FAKEGLIB_API void g_date_time_unref(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(referenceMutex);
	dateTime->referenceCount--;
	if(dateTime->referenceCount == 0) {
		delete dateTime;
	}
}

FAKEGLIB_API GDateTime *g_date_time_ref(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(referenceMutex);
	dateTime->referenceCount++;
	return dateTime;
}

FAKEGLIB_API GDateTime *g_date_time_new_now_local()
{
	GDateTime *dateTime = new GDateTime{};
	dateTime->time = std::chrono::system_clock::now();
	dateTime->timeT = std::chrono::system_clock::to_time_t(dateTime->time);
	dateTime->isUtc = false;
	dateTime->referenceCount = 1;
	return dateTime;
}

FAKEGLIB_API GDateTime *g_date_time_new_now_utc()
{
	GDateTime *dateTime = new GDateTime{};
	dateTime->time = std::chrono::system_clock::now();
	dateTime->timeT = std::chrono::system_clock::to_time_t(dateTime->time);
	dateTime->isUtc = true;
	dateTime->referenceCount = 1;
	return dateTime;
}

FAKEGLIB_API gint g_date_time_get_year(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(chronoMutex);

	struct tm *tm;
	if(dateTime->isUtc) {
		tm = std::gmtime(&dateTime->timeT);
	} else {
		tm = std::localtime(&dateTime->timeT);
	}

	return tm->tm_year + 1900;
}

FAKEGLIB_API gint g_date_time_get_month(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(chronoMutex);

	struct tm *tm;
	if(dateTime->isUtc) {
		tm = std::gmtime(&dateTime->timeT);
	} else {
		tm = std::localtime(&dateTime->timeT);
	}

	return tm->tm_mon + 1;
}

FAKEGLIB_API gint g_date_time_get_day_of_month(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(chronoMutex);

	struct tm *tm;
	if(dateTime->isUtc) {
		tm = std::gmtime(&dateTime->timeT);
	} else {
		tm = std::localtime(&dateTime->timeT);
	}

	return tm->tm_mday;
}

FAKEGLIB_API gint g_date_time_get_hour(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(chronoMutex);

	struct tm *tm;
	if(dateTime->isUtc) {
		tm = std::gmtime(&dateTime->timeT);
	} else {
		tm = std::localtime(&dateTime->timeT);
	}

	return tm->tm_hour;
}

FAKEGLIB_API gint g_date_time_get_minute(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(chronoMutex);

	struct tm *tm;
	if(dateTime->isUtc) {
		tm = std::gmtime(&dateTime->timeT);
	} else {
		tm = std::localtime(&dateTime->timeT);
	}

	return tm->tm_min;
}

FAKEGLIB_API gint g_date_time_get_second(GDateTime *dateTime)
{
	std::lock_guard<std::mutex> lock(chronoMutex);

	struct tm *tm;
	if(dateTime->isUtc) {
		tm = std::gmtime(&dateTime->timeT);
	} else {
		tm = std::localtime(&dateTime->timeT);
	}

	if(tm->tm_sec >= 60) {
		return 59; // glib API doesn't know leap seconds
	} else {
		return tm->tm_sec;
	}
}

FAKEGLIB_API gint g_date_time_get_microsecond(GDateTime *datetime)
{
	std::chrono::system_clock::duration tp = datetime->time.time_since_epoch();
	tp -= std::chrono::duration_cast<std::chrono::seconds>(tp);
	return static_cast<gint>(tp / std::chrono::microseconds{1});
}

FAKEGLIB_API gint64 g_date_time_to_unix(GDateTime *datetime)
{
	return std::chrono::seconds(datetime->timeT).count();
}
