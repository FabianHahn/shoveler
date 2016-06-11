#ifndef SHOVELER_LOG_H
#define SHOVELER_LOG_H

#include <stdio.h> // FILE

/**
 * Log level enum describing possible logging modes
 */
typedef enum
{
	/** Debugging information that may be extremely verbose. Not for general use! */
	SHOVELER_LOG_LEVEL_TRACE = 1,
	/** Unimportant verbose information */
	SHOVELER_LOG_LEVEL_INFO = 2,
	/** The function has an unexpected state but can go on with the work */
	SHOVELER_LOG_LEVEL_WARNING = 4,
	/** The function has an unexpected state and can not end the work */
	SHOVELER_LOG_LEVEL_ERROR = 8,
	/** No logging */
	SHOVELER_LOG_LEVEL_NONE = 0,
	/** Log errors and up */
	SHOVELER_LOG_LEVEL_ERROR_UP = 8,
	/** Log warnings and up */
	SHOVELER_LOG_LEVEL_WARNING_UP = 12,
	/** Log infos and up */
	SHOVELER_LOG_LEVEL_INFO_UP = 14,
	/** Log everything! The sky is the limit! */
	SHOVELER_LOG_LEVEL_ALL = 15
} ShovelerLogLevel;

void shovelerLogInitWithLocation(const char *file, ShovelerLogLevel level, FILE *channel);

#define shovelerLogInit(LEVEL, CHANNEL) shovelerLogInitWithLocation(__FILE__, LEVEL, CHANNEL);

void shovelerLogTerminate();

void shovelerLogMessage(const char *file, int line, ShovelerLogLevel level, const char *message, ...);

#ifdef SHOVELER_DISABLE_TRACE_LOGGING
#define shovelerLogTrace(...) ((void) 0)
#else
#define shovelerLogTrace(...) shovelerLogMessage(__FILE__, __LINE__, SHOVELER_LOG_LEVEL_TRACE, __VA_ARGS__)
#endif

#define shovelerLogInfo(...) shovelerLogMessage(__FILE__, __LINE__, SHOVELER_LOG_LEVEL_INFO, __VA_ARGS__)
#define shovelerLogWarning(...) shovelerLogMessage(__FILE__, __LINE__, SHOVELER_LOG_LEVEL_WARNING, __VA_ARGS__)
#define shovelerLogError(...) shovelerLogMessage(__FILE__, __LINE__, SHOVELER_LOG_LEVEL_ERROR, __VA_ARGS__)

#endif
