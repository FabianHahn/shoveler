#include "shoveler/global.h"

#include <GLFW/glfw3.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef _WIN32
static void handleUbsan(int signo, siginfo_t* si, void* uc);
#endif
static void handleGlfwErrorMessage(int errorCode, const char* message);

static int referenceCount = 0;
static ShovelerGlobalContext globalContext;

bool shovelerGlobalInit() {
  if (referenceCount == 0) {
    shovelerLogInfo("Initializing global context.");

#ifndef _WIN32
    struct sigaction sa = {};
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handleUbsan;
#if defined(__aarch64__)
    sigaction(SIGTRAP, &sa, nullptr);
#elif defined(__x86_64__)
    sigaction(SIGILL, &sa, NULL);
#endif
#endif

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

#ifndef _WIN32
static void handleUbsan(int signo, siginfo_t* si, void* uc) {
#ifdef __aarch64__
  if (signo == SIGTRAP && si && si->si_addr) {
    uint32_t insn;
    memcpy(&insn, si->si_addr, sizeof(insn));
    if ((insn & 0xffe0001f) == 0xd4200000 && ((insn >> 13) & 255) == 'U') {
      char buf[64];
      int len = snprintf(
          buf,
          sizeof buf,
          "TRAP(%u) at %p, possibly UBSan error\n",
          (insn >> 5 & 255),
          si->si_addr);
      write(STDERR_FILENO, buf, len);
    }
  }
#endif
#ifdef __x86_64__
  if (signo == SIGILL && si && si->si_code == ILL_ILLOPN && si->si_addr) {
    const unsigned char* pc = (const unsigned char*) si->si_addr;
    if (pc[0] == 0x67 && pc[1] == 0x0f && pc[2] == 0xb9 && pc[3] == 0x40) {
      char buf[64];
      int len = snprintf(buf, sizeof buf, "UD1 at %p, possibly UBSan error\n", pc);

      write(STDERR_FILENO, buf, len);
    }
  }
#endif
  abort();
}
#endif

static void handleGlfwErrorMessage(int errorCode, const char* message) {
  shovelerLogMessage("glfw", 0, SHOVELER_LOG_LEVEL_ERROR, "error code %d: %s", errorCode, message);
}
