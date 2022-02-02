#ifndef SHOVELER_EXECUTOR_H
#define SHOVELER_EXECUTOR_H

#include <stdbool.h> // bool

#include <glib.h>

typedef void (ShovelerExecutorCallbackFunction)(void *userData);

typedef struct ShovelerExecutorCallbackStruct {
	gint64 expiry;
	int intervalMs;
	ShovelerExecutorCallbackFunction *callbackFunction;
	void *userData;
} ShovelerExecutorCallback;

typedef struct ShovelerExecutorStruct {
	gint64 lastUpdate;
	/** set of (ShovelerExecutorCallback *) */
	GHashTable *callbacks;
} ShovelerExecutor;

ShovelerExecutor *shovelerExecutorCreateDirect();
void shovelerExecutorUpdate(ShovelerExecutor *executor, gint64 elapsedUs);
void shovelerExecutorUpdateNow(ShovelerExecutor *executor);
ShovelerExecutorCallback *shovelerExecutorSchedulePeriodic(ShovelerExecutor *executor, int timeoutMs, int intervalMs, ShovelerExecutorCallbackFunction *callbackFunction, void *userData);
bool shovelerExecutorRemoveCallback(ShovelerExecutor *executor, ShovelerExecutorCallback *callback);
void shovelerExecutorFree(ShovelerExecutor *executor);

static inline ShovelerExecutorCallback *shovelerExecutorSchedule(ShovelerExecutor *executor, int timeoutMs, ShovelerExecutorCallbackFunction *callbackFunction, void *userData)
{
	return shovelerExecutorSchedulePeriodic(executor, timeoutMs, 0, callbackFunction, userData);
}

#endif
