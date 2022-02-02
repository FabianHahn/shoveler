#include <stdlib.h> // malloc, free

#include <glib.h>

#include "shoveler/executor.h"
#include "shoveler/log.h"

static void freeCallback(void *callbackPointer);

ShovelerExecutor *shovelerExecutorCreateDirect()
{
	ShovelerExecutor *executor = malloc(sizeof(ShovelerExecutor));
	executor->lastUpdate = g_get_monotonic_time();
	executor->callbacks = g_hash_table_new_full(g_direct_hash, g_direct_equal, freeCallback, NULL);
	return executor;
}

void shovelerExecutorUpdate(ShovelerExecutor *executor, gint64 elapsedUs)
{
	executor->lastUpdate += elapsedUs;

	GHashTableIter iter;
	ShovelerExecutorCallback *callback;
	g_hash_table_iter_init(&iter, executor->callbacks);
	while(g_hash_table_iter_next(&iter, (gpointer *) &callback, NULL)) {
		if(executor->lastUpdate >= callback->expiry) {
			callback->callbackFunction(callback->userData);

			if(callback->intervalMs > 0) {
				callback->expiry = executor->lastUpdate + callback->intervalMs * 1000;
			} else {
				g_hash_table_iter_remove(&iter);
			}
		}
	}
}

void shovelerExecutorUpdateNow(ShovelerExecutor *executor)
{
	shovelerExecutorUpdate(executor, g_get_monotonic_time() - executor->lastUpdate);
}

ShovelerExecutorCallback *shovelerExecutorSchedulePeriodic(ShovelerExecutor *executor, int timeoutMs, int intervalMs, ShovelerExecutorCallbackFunction *callbackFunction, void *userData)
{
	ShovelerExecutorCallback *callback = malloc(sizeof(ShovelerExecutorCallback));
	callback->expiry = executor->lastUpdate + timeoutMs * 1000;
	callback->intervalMs = intervalMs;
	callback->callbackFunction = callbackFunction;
	callback->userData = userData;

	g_hash_table_add(executor->callbacks, callback);
	return callback;
}

bool shovelerExecutorRemoveCallback(ShovelerExecutor *executor, ShovelerExecutorCallback *callback)
{
	return g_hash_table_remove(executor->callbacks, callback);
}

void shovelerExecutorFree(ShovelerExecutor *executor)
{
	g_hash_table_destroy(executor->callbacks);
	free(executor);
}

static void freeCallback(void *callbackPointer)
{
	free(callbackPointer);
}
