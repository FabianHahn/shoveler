#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "shoveler/component_type_indexer.h"
}

namespace {
const char* testComponentType1 = "component_type_1";
const char* testComponentType2 = "component_type_2";
const char* testComponentType3 = "component_type_3";
} // namespace

TEST(ShovelerComponentTypeIndexerTest, test) {
  ShovelerComponentTypeIndexer* componentTypeIndexer = shovelerComponentTypeIndexerCreate();
  shovelerComponentTypeIndexerAddComponentType(componentTypeIndexer, testComponentType3);
  shovelerComponentTypeIndexerAddComponentType(componentTypeIndexer, testComponentType1);

  int componentIndex1 =
      shovelerComponentTypeIndexerFromId(componentTypeIndexer, testComponentType1);
  int componentIndex2 =
      shovelerComponentTypeIndexerFromId(componentTypeIndexer, testComponentType2);
  int componentIndex3 =
      shovelerComponentTypeIndexerFromId(componentTypeIndexer, testComponentType3);
  ASSERT_GE(componentIndex1, 0);
  ASSERT_LE(componentIndex1, 1);
  ASSERT_LT(componentIndex2, 0);
  ASSERT_GE(componentIndex3, 0);
  ASSERT_LE(componentIndex3, 1);
  ASSERT_NE(componentIndex1, componentIndex3);

  const char* componentType1 =
      shovelerComponentTypeIndexerToId(componentTypeIndexer, componentIndex1);
  const char* componentType2 =
      shovelerComponentTypeIndexerToId(componentTypeIndexer, componentIndex2);
  const char* componentType3 =
      shovelerComponentTypeIndexerToId(componentTypeIndexer, componentIndex3);
  ASSERT_EQ(componentType1, testComponentType1);
  ASSERT_EQ(componentType2, nullptr);
  ASSERT_EQ(componentType3, testComponentType3);

  shovelerComponentTypeIndexerFree(componentTypeIndexer);
}
