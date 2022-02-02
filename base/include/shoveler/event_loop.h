#ifndef SHOVELER_EVENT_LOOP_H
#define SHOVELER_EVENT_LOOP_H

#include <glib.h>

typedef struct ShovelerEventLoopStruct ShovelerEventLoop;
typedef struct ShovelerExecutorStruct ShovelerExecutor;

typedef void (ShovelerEventLoopTickFunction)(ShovelerEventLoop* eventLoop, gint64 dtUs, void* userData);

typedef struct ShovelerEventLoopStruct {
	ShovelerExecutor* executor;
	gint64 tickPeriodUs;
	ShovelerEventLoopTickFunction* tick;
	void* userData;
	gint64 tickStartTime;
	gint64 lastTickTime;
} ShovelerEventLoop;

ShovelerEventLoop* shovelerEventLoopCreate(
	gint64 tickRateHz, ShovelerEventLoopTickFunction* tick, void* userData);
void shovelerEventLoopTick(ShovelerEventLoop* eventLoop);
void shovelerEventLoopFree(ShovelerEventLoop* eventLoop);

#endif
