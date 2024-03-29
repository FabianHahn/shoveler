#include <gtest/gtest.h>

extern "C" {
#include "shoveler/log.h"
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_ALL, stdout);
  int result = RUN_ALL_TESTS();
  shovelerLogTerminate();

  return result;
}
