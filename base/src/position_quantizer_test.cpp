extern "C" {
#include "shoveler/position_quantizer.h"
}

#include <gtest/gtest.h>

static const float eps = 1e-3f;

static bool operator==(const ShovelerVector2& a, const ShovelerVector2& b) {
  return shovelerVector2Equals(a, b, eps);
}

static std::ostream& operator<<(std::ostream& stream, const ShovelerVector2& v) {
  return stream << "(" << v.values[0] << ", " << v.values[1] << ")";
}

TEST(PositionQuantizer, QuantizeDequantize) {
  ShovelerVector2 worldMin = shovelerVector2(0.0f, -100.0f);
  ShovelerVector2 worldMax = shovelerVector2(50.0f, 100.0f);

  ShovelerPositionQuantizer quantizer = shovelerPositionQuantizer(worldMin, worldMax);

  uint64_t quantizedX;
  uint64_t quantizedY;

  // minimum
  shovelerPositionQuantizerFromWorld(&quantizer, worldMin, &quantizedX, &quantizedY);
  ASSERT_EQ(quantizedX, 0);
  ASSERT_EQ(quantizedY, 0);

  // maximum
  shovelerPositionQuantizerFromWorld(&quantizer, worldMax, &quantizedX, &quantizedY);
  ASSERT_EQ(quantizedX, UINT64_MAX);
  ASSERT_EQ(quantizedY, UINT64_MAX);

  auto quantize_dequantize = [&](ShovelerVector2 input) {
    shovelerPositionQuantizerFromWorld(&quantizer, input, &quantizedX, &quantizedY);
    return shovelerPositionQuantizerToWorld(&quantizer, quantizedX, quantizedY);
  };

  ShovelerVector2 middle = shovelerVector2(0.0f, 0.0f);
  ShovelerVector2 test1 = shovelerVector2(13.37f, -2.7f);
  ShovelerVector2 test2 = shovelerVector2(42.42f, -90.009f);
  ASSERT_EQ(middle, quantize_dequantize(middle));
  ASSERT_EQ(test1, quantize_dequantize(test1));
  ASSERT_EQ(test2, quantize_dequantize(test2));
}
