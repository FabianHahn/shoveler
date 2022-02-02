#include "shoveler/event_loop.h"

#include <stdlib.h>
#include <glib.h>
#include "shoveler/executor.h"

ShovelerEventLoop* shovelerEventLoopCreate(
	gint64 tickRateHz, ShovelerEventLoopTickFunction* tick, void* userData)
{
	ShovelerEventLoop* eventLoop = malloc(sizeof(ShovelerEventLoop));
	eventLoop->executor = shovelerExecutorCreateDirect();
	eventLoop->tickPeriodUs = 1000 * 1000 / tickRateHz;
	eventLoop->tick = tick;
	eventLoop->userData = userData;
	eventLoop->tickStartTime = g_get_monotonic_time();
	eventLoop->lastTickTime = eventLoop->tickStartTime;
}

void shovelerEventLoopTick(ShovelerEventLoop* eventLoop)
{
	gint64 dtUs = eventLoop->tickStartTime - eventLoop->lastTickTime;

	eventLoop->tick(eventLoop, dtUs, eventLoop->userData);
	shovelerExecutorUpdate(eventLoop->executor, dtUs);

	gint64 tickEndTime = g_get_monotonic_time();
	gint64 tickTimeUs = tickEndTime - eventLoop->tickStartTime;
	gint64 remainingTimeUs = eventLoop->tickPeriodUs - tickTimeUs;
	if (remainingTimeUs > 0) {
		g_usleep(remainingTimeUs);
		tickEndTime = g_get_monotonic_time(); // readjust after sleeping
	}

	eventLoop->lastTickTime = eventLoop->tickStartTime;
	eventLoop->tickStartTime = tickEndTime;
}

void shovelerEventLoopFree(ShovelerEventLoop* eventLoop)
{
	shovelerExecutorFree(eventLoop->executor);
	free(eventLoop);
}
