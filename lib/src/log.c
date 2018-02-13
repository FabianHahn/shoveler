#include <stdbool.h> // bool
#include <stdio.h> // FILE, fprintf, fflush
#include <stdlib.h> // free
#include <stddef.h> // NULL
#include <string.h> // strdup, strrchr

#include <glib.h>
#include <GLFW/glfw3.h>

#include "shoveler/log.h"

static void handleGlfwErrorMessage(int errorCode, const char *message);
static void logHandler(const char *file, int line, ShovelerLogLevel level, const char *message);
static bool shouldLog(ShovelerLogLevel level);
static char *formatLogMessage(const char *file, int line, ShovelerLogLevel level, const char *message);
static const char *getStaticLogLevelName(ShovelerLogLevel level);

static char *logLocationPrefix = NULL;
static ShovelerLogLevel logLevel;
static FILE *logChannel;
static ShovelerLogMessageCallbackFunction *logCallbackFunction = &logHandler;

void shovelerLogInitWithLocation(const char *location, ShovelerLogLevel level, FILE *channel)
{
	logLocationPrefix = strdup(location);
	char *lastSeparator = strrchr(logLocationPrefix, '/');
	if(lastSeparator != NULL) {
		*(++lastSeparator) = '\0';
	}

	logLevel = level;
	logChannel = channel;

	glfwSetErrorCallback(handleGlfwErrorMessage);
}

void shovelerLogInitWithCallback(ShovelerLogLevel level, ShovelerLogMessageCallbackFunction *callbackFunction)
{
	logLevel = level;
	logChannel = NULL;
	logCallbackFunction = callbackFunction;

	glfwSetErrorCallback(handleGlfwErrorMessage);
}

void shovelerLogTerminate()
{
	free(logLocationPrefix);
}

void shovelerLogMessage(const char *file, int line, ShovelerLogLevel level, const char *message, ...)
{
	if(shouldLog(level)) {
		va_list va;
		va_start(va, message);
		GString *assembled = g_string_new("");
		g_string_append_vprintf(assembled, message, va);
		logCallbackFunction(file, line, level, assembled->str);
		g_string_free(assembled, true);
	}
}

static void handleGlfwErrorMessage(int errorCode, const char *message)
{
	shovelerLogMessage("glfw", 0, SHOVELER_LOG_LEVEL_ERROR, "error code %d: %s", errorCode, message);
}

static void logHandler(const char *file, int line, ShovelerLogLevel level, const char *message)
{
	if(logChannel != NULL) {
		char *formatted = formatLogMessage(file, line, level, message);
		fprintf(logChannel, "%s\n", formatted);
		g_free(formatted);
		fflush(logChannel);
	}
}

static bool shouldLog(ShovelerLogLevel level)
{
	return logLevel & level;
}

static char *formatLogMessage(const char *file, int line, ShovelerLogLevel level, const char *message)
{
	const char *strippedLocation = file;
	if(strncmp(logLocationPrefix, file, strlen(logLocationPrefix)) == 0) {
		strippedLocation += strlen(logLocationPrefix);
	}

	GDateTime *now = g_date_time_new_now_local();
	GString *result = g_string_new("");

	g_string_append_printf(result, "[%02d:%02d:%02d] (%s:%s:%d) %s", g_date_time_get_hour(now), g_date_time_get_minute(now), g_date_time_get_second(now), getStaticLogLevelName(level), strippedLocation, line, message);

	g_date_time_unref(now);
	return g_string_free(result, false);
}

static const char *getStaticLogLevelName(ShovelerLogLevel level)
{
	if(level & SHOVELER_LOG_LEVEL_TRACE) {
		return "trace";
	} else if(level & SHOVELER_LOG_LEVEL_INFO) {
		return "info";
	} else if(level & SHOVELER_LOG_LEVEL_WARNING) {
		return "warning";
	} else if(level & SHOVELER_LOG_LEVEL_ERROR) {
		return "error";
	} else {
		return "unknown";
	}
}
