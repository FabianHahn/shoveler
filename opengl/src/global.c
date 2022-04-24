#include "shoveler/global.h"

#include <GLFW/glfw3.h>

static void handleGlfwErrorMessage(int errorCode, const char* message);

static int referenceCount = 0;
static ShovelerGlobalContext globalContext;

bool shovelerGlobalInit() {
  if (referenceCount == 0) {
    shovelerLogInfo("Initializing global context.");

    globalContext.games = g_hash_table_new(g_direct_hash, g_direct_equal);

    glfwSetErrorCallback(handleGlfwErrorMessage);

    if (!glfwInit()) {
      shovelerLogError("Failed to initialize glfw.");
      return false;
    }
  }

  referenceCount++;
  return true;
}

ShovelerGlobalContext* shovelerGlobalGetContext() { return &globalContext; }

void shovelerGlobalUninit() {
  referenceCount--;

  if (referenceCount == 0) {
    shovelerLogInfo("Destroying global context.");
    glfwTerminate();
    g_hash_table_destroy(globalContext.games);
  }
}

static void handleGlfwErrorMessage(int errorCode, const char* message) {
  shovelerLogMessage("glfw", 0, SHOVELER_LOG_LEVEL_ERROR, "error code %d: %s", errorCode, message);
}
