#include <GLFW/glfw3.h>

#include "shoveler/log.h"
#include "shoveler/global.h"

static int referenceCount = 0;
static ShovelerGlobalContext globalContext;

bool shovelerGlobalInit(ShovelerLogLevel logLevel, FILE *logChannel)
{
	if(referenceCount == 0) {
		shovelerLogInit(logLevel, logChannel);
		shovelerLogInfo("Initializing global context.");

		globalContext.games = g_hash_table_new(g_direct_hash, g_direct_equal);

		if(!glfwInit()) {
			shovelerLogError("Failed to initialize glfw.");
			return false;
		}
	}

	referenceCount++;
	return true;
}

ShovelerGlobalContext *shovelerGlobalGetContext()
{
	return &globalContext;
}

void shovelerGlobalUninit()
{
	referenceCount--;

	if(referenceCount == 0) {
		shovelerLogInfo("Destroying global context.");
		glfwTerminate();
		g_hash_table_destroy(globalContext.games);
		shovelerLogTerminate();
	}
}