#include "shoveler/log.h"

#include <glib.h>
#include <stdbool.h> // bool
#include <stddef.h> // NULL
#include <stdio.h> // FILE, fprintf, fflush
#include <stdlib.h> // free
#include <string.h> // strdup, strstr

static void logHandler(const char* file, int line, ShovelerLogLevel level, const char* message);
static bool shouldLog(ShovelerLogLevel level);
static char* formatLogMessage(
    const char* file, int line, ShovelerLogLevel level, const char* message);
static const char* getStaticLogLevelName(ShovelerLogLevel level);

static char* logLocationPrefix = NULL;
static ShovelerLogLevel logLevel;
static FILE* logChannel;
static ShovelerLogMessageCallbackFunction* logCallbackFunction = &logHandler;

void shovelerLogInit(const char* locationPrefix, ShovelerLogLevel level, FILE* channel) {
  logLocationPrefix = strdup(locationPrefix);
  logLevel = level;
  logChannel = channel;
}

void shovelerLogInitWithCallback(
    ShovelerLogLevel level, ShovelerLogMessageCallbackFunction* callbackFunction) {
  logLevel = level;
  logChannel = NULL;
  logCallbackFunction = callbackFunction;
}

void shovelerLogTerminate() { free(logLocationPrefix); }

void shovelerLogMessage(
    const char* file, int line, ShovelerLogLevel level, const char* message, ...) {
  if (shouldLog(level)) {
    va_list va;
    va_start(va, message);
    GString* assembled = g_string_new("");
    g_string_append_vprintf(assembled, message, va);
    logCallbackFunction(file, line, level, assembled->str);
    g_string_free(assembled, true);
  }
}

static void logHandler(const char* file, int line, ShovelerLogLevel level, const char* message) {
  if (logChannel != NULL) {
    char* formatted = formatLogMessage(file, line, level, message);
    fprintf(logChannel, "%s\n", formatted);
    g_free(formatted);
    fflush(logChannel);
  }
}

static bool shouldLog(ShovelerLogLevel level) { return logLevel & level; }

static char* formatLogMessage(
    const char* file, int line, ShovelerLogLevel level, const char* message) {
  const char* strippedLocation = strstr(file, logLocationPrefix);
  if (strippedLocation != NULL) {
    strippedLocation += strlen(logLocationPrefix);
  } else {
    strippedLocation = file;
  }

  GDateTime* now = g_date_time_new_now_local();
  GString* result = g_string_new("");

  g_string_append_printf(
      result,
      "[%02d:%02d:%02d.%03d] (%s:%s:%d) %s",
      g_date_time_get_hour(now),
      g_date_time_get_minute(now),
      g_date_time_get_second(now),
      g_date_time_get_microsecond(now) / 1000,
      getStaticLogLevelName(level),
      strippedLocation,
      line,
      message);

  g_date_time_unref(now);
  return g_string_free(result, false);
}

static const char* getStaticLogLevelName(ShovelerLogLevel level) {
  if (level & SHOVELER_LOG_LEVEL_TRACE) {
    return "trace";
  } else if (level & SHOVELER_LOG_LEVEL_INFO) {
    return "info";
  } else if (level & SHOVELER_LOG_LEVEL_WARNING) {
    return "warning";
  } else if (level & SHOVELER_LOG_LEVEL_ERROR) {
    return "error";
  } else {
    return "unknown";
  }
}
