#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <type_traits>

extern "C" {
#include "shoveler/types.h"
}

const float EPS = 1e-6f;

template <typename T>
struct VectorValueTrait : std::false_type {};

template <>
struct VectorValueTrait<ShovelerVector2> : std::true_type {
  static const int SIZE = 2;
};
template <>
struct VectorValueTrait<ShovelerVector3> : std::true_type {
  static const int SIZE = 3;
};
template <>
struct VectorValueTrait<ShovelerVector4> : std::true_type {
  static const int SIZE = 4;
};
template <>
struct VectorValueTrait<ShovelerMatrix> : std::true_type {
  static const int SIZE = 16;
};

template <typename T, typename std::enable_if<VectorValueTrait<T>::value>::type...>
static bool operator==(const T& a, const T& b) {
  for (int i = 0; i < VectorValueTrait<T>::SIZE; i++) {
    if (fabsf(a.values[i] - b.values[i]) > EPS) {
      return false;
    }
  }
  return true;
}

template <typename T, typename std::enable_if<VectorValueTrait<T>::value>::type...>
static std::ostream& operator<<(std::ostream& stream, const T& v) {
  stream << "[";
  for (int i = 0; i < VectorValueTrait<T>::SIZE; i++) {
    if (i > 0) {
      stream << ", ";
    }
    stream << v.values[i];
  }
  stream << "]";
  return stream;
}

static ShovelerVector2 testVector2 = {1, 2};
static ShovelerVector3 testVector3 = {1, 2, 3};
static ShovelerVector4 testVector4 = {1, 2, 3, 4};
static ShovelerMatrix testMatrix2 = {1, 2, 3, 0, 5, 6, 7, 0, 0, 0, 1, 0, 0, 0, 0, 1};
static ShovelerMatrix testMatrix3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 1};
static ShovelerMatrix testMatrix4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

TEST(vector, cross) {
  static ShovelerVector3 a = {1, -1, 0};
  static ShovelerVector3 b = {1, 1, 0};
  static ShovelerVector3 solution = {0, 0, 2};

  ShovelerVector3 result = shovelerVector3Cross(a, b);
  ASSERT_EQ(result, solution);
}

TEST(vector, crossSelf) {
  static ShovelerVector3 solution = {0, 0, 0};

  ShovelerVector3 result = shovelerVector3Cross(testVector3, testVector3);
  ASSERT_EQ(result, solution);
}

TEST(vector, dot) {
  static ShovelerVector3 b = {9, 8, 7};
  static float solution = 46;

  float result = shovelerVector3Dot(testVector3, b);
  ASSERT_EQ(result, solution);
}

TEST(vector, dotSelf) {
  static float solution = 14;

  float result = shovelerVector3Dot(testVector3, testVector3);
  ASSERT_EQ(result, solution);
}

TEST(vector, normalize) {
  static ShovelerVector3 solution = {0.267261f, 0.534522f, 0.801784f};

  ShovelerVector3 result = shovelerVector3Normalize(testVector3);
  ASSERT_EQ(result, solution);
}

TEST(matrix, transpose) {
  ShovelerMatrix result = shovelerMatrixTranspose(testMatrix4);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ASSERT_EQ(shovelerMatrixGet(result, i, j), shovelerMatrixGet(testMatrix4, j, i))
          << "transposed matrix element at (" << i << ", " << j
          << ") must match original element at (" << j << ", " << i << ")";
    }
  }
}

TEST(matrix, transposeTwice) {
  ShovelerMatrix result = shovelerMatrixTranspose(shovelerMatrixTranspose(testMatrix4));
  ASSERT_EQ(result, testMatrix4);
}

TEST(matrix, multiply) {
  static ShovelerMatrix solution = {
      90, 100, 110, 120, 202, 228, 254, 280, 314, 356, 398, 440, 426, 484, 542, 600};

  ShovelerMatrix result = shovelerMatrixMultiply(testMatrix4, testMatrix4);
  ASSERT_EQ(result, solution);
}

TEST(matrix, multiplyZeroLeft) {
  ShovelerMatrix result = shovelerMatrixMultiply(shovelerMatrixZero, testMatrix4);
  ASSERT_EQ(result, shovelerMatrixZero);
}

TEST(matrix, multiplyZeroRight) {
  ShovelerMatrix result = shovelerMatrixMultiply(testMatrix4, shovelerMatrixZero);
  ASSERT_EQ(result, shovelerMatrixZero);
}

TEST(matrix, multiplyIdentityLeft) {
  ShovelerMatrix result = shovelerMatrixMultiply(shovelerMatrixIdentity, testMatrix4);
  ASSERT_EQ(result, testMatrix4);
}

TEST(matrix, multiplyIdentityRight) {
  ShovelerMatrix result = shovelerMatrixMultiply(testMatrix4, shovelerMatrixIdentity);
  ASSERT_EQ(result, testMatrix4);
}

TEST(matrix, multiplyVector2) {
  static ShovelerVector2 solution = {8, 24};

  ShovelerVector2 result = shovelerMatrixMultiplyVector2(testMatrix2, testVector2);
  ASSERT_EQ(result, solution);
}

TEST(matrix, multiplyVector3) {
  static ShovelerVector3 solution = {18, 46, 74};

  ShovelerVector3 result = shovelerMatrixMultiplyVector3(testMatrix3, testVector3);
  ASSERT_EQ(result, solution);
}

TEST(matrix, multiplyVector4) {
  static ShovelerVector4 solution = {30, 70, 110, 150};

  ShovelerVector4 result = shovelerMatrixMultiplyVector4(testMatrix4, testVector4);
  ASSERT_EQ(result, solution);
}
