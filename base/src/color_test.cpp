#include <gtest/gtest.h>

#include <map>
#include <string>
#include <utility>

extern "C" {
#include "shoveler/color.h"
}

static bool operator==(const ShovelerColor& a, const ShovelerColor& b) {
  return a.r == b.r && a.g == b.g && a.b == b.b;
}

static std::ostream& operator<<(std::ostream& stream, const ShovelerColor& v) {
  return stream << "(" << (int) v.r << ", " << (int) v.g << ", " << (int) v.b << ")";
}

struct HsvColor {
  float h;
  float s;
  float v;
};

TEST(color, colorFromHsv) {
  std::map<std::string, std::pair<HsvColor, ShovelerColor>> testCases{
      {"black", {{0.0f, 0.0f, 0.0f}, {0, 0, 0}}},
      {"white", {{0.0f, 0.0f, 1.0f}, {255, 255, 255}}},
      {"red", {{0.0f, 1.0f, 1.0f}, {255, 0, 0}}},
      {"lime", {{1.0f / 3.0f, 1.0f, 1.0f}, {0, 255, 0}}},
      {"blue", {{2.0f / 3.0f, 1.0f, 1.0f}, {0, 0, 255}}},
      {"yellow", {{1.0f / 6.0f, 1.0f, 1.0f}, {255, 255, 0}}},
      {"cyan", {{0.5f, 1.0f, 1.0f}, {0, 255, 255}}},
      {"magenta", {{5.0f / 6.0f, 1.0f, 1.0f}, {255, 0, 255}}},
      {"silver", {{0.0f, 0.0f, 0.75f}, {191, 191, 191}}},
      {"gray", {{0.0f, 0.0f, 0.5f}, {128, 128, 128}}},
      {"maroon", {{0.0f, 1.0f, 0.5f}, {128, 0, 0}}},
      {"olive", {{1.0f / 6.0f, 1.0f, 0.5f}, {128, 128, 0}}},
      {"green", {{1.0f / 3.0f, 1.0f, 0.5f}, {0, 128, 0}}},
      {"purple", {{5.0f / 6.0f, 1.0f, 0.5f}, {128, 0, 128}}},
      {"teal", {{0.5f, 1.0f, 0.5f}, {0, 128, 128}}},
      {"navy", {{2.0f / 3.0f, 1.0f, 0.5f}, {0, 0, 128}}}};

  for (const auto& testCase : testCases) {
    const std::string& name = testCase.first;
    const HsvColor& input = testCase.second.first;
    const ShovelerColor& solution = testCase.second.second;

    ShovelerColor result = shovelerColorFromHsv(input.h, input.s, input.v);
    ASSERT_EQ(result, solution) << "result should be " << name;
  }
}
