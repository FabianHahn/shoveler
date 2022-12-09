#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "shoveler/component_type_indexer.h"
#include "shoveler/server_op.h"
}

namespace {
const char* testComponentType1 = "component_type_1";
const char* testComponentType2 = "component_type_2";
const char* testComponentType3 = "component_type_3";

class ShovelerServerOpTest : public ::testing::Test {
public:
  virtual void SetUp() {
    componentTypeIndexer = shovelerComponentTypeIndexerCreate();
    shovelerComponentTypeIndexerAddComponentType(componentTypeIndexer, testComponentType3);
    shovelerComponentTypeIndexerAddComponentType(componentTypeIndexer, testComponentType1);
  }

  virtual void TearDown() { shovelerComponentTypeIndexerFree(componentTypeIndexer); }

  void TestSerialization(const ShovelerServerOp* serverOp) {
    GString* output = g_string_new("");
    bool serialized = shovelerServerOpSerialize(serverOp, componentTypeIndexer, output);
    ASSERT_TRUE(serialized);
    ASSERT_GT(output->len, 0);

    ShovelerServerOpWithData deserializedServerOp;
    shovelerServerOpInitWithData(&deserializedServerOp, /* inputServerOp */ nullptr);
    int readIndex = 0;
    bool deserialized = shovelerServerOpDeserialize(
        &deserializedServerOp,
        componentTypeIndexer,
        (unsigned char*) output->str,
        (int) output->len,
        &readIndex);
    ASSERT_TRUE(deserialized);
    ASSERT_EQ(readIndex, output->len);
    ASSERT_TRUE(shovelerServerOpEquals(serverOp, &deserializedServerOp.op));

    shovelerServerOpClearWithData(&deserializedServerOp);
    g_string_free(output, /* freeSegment */ true);
  }

  ShovelerComponentTypeIndexer* componentTypeIndexer;
};

} // namespace

TEST_F(ShovelerServerOpTest, SerializeAddEntityInterest) {
  ShovelerServerOp serverOp;
  serverOp.type = SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST;
  serverOp.addEntityInterest.entityId = 42;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&serverOp));
}

TEST_F(ShovelerServerOpTest, SerializeRemoveEntityInterest) {
  ShovelerServerOp serverOp;
  serverOp.type = SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST;
  serverOp.removeEntityInterest.entityId = 9001;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&serverOp));
}

TEST_F(ShovelerServerOpTest, SerializeUpdateComponent) {
  ShovelerComponentFieldValue fieldValue;
  fieldValue.type = SHOVELER_COMPONENT_FIELD_TYPE_STRING;
  fieldValue.isSet = true;
  fieldValue.stringValue = (char*) "the bird is the word"; // not modified

  ShovelerServerOp serverOp;
  serverOp.type = SHOVELER_SERVER_OP_UPDATE_COMPONENT;
  serverOp.updateComponent.entityId = 42;
  serverOp.updateComponent.componentTypeId = testComponentType3;
  serverOp.updateComponent.fieldId = 17;
  serverOp.updateComponent.fieldValue = &fieldValue;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&serverOp));
}

TEST_F(ShovelerServerOpTest, FailSerializeInvalidComponentType) {
  ShovelerComponentFieldValue fieldValue;
  fieldValue.type = SHOVELER_COMPONENT_FIELD_TYPE_STRING;
  fieldValue.isSet = true;
  fieldValue.stringValue = (char*) "the bird is the word"; // not modified

  ShovelerServerOp serverOp;
  serverOp.type = SHOVELER_SERVER_OP_UPDATE_COMPONENT;
  serverOp.updateComponent.entityId = 42;
  serverOp.updateComponent.componentTypeId = testComponentType2;
  serverOp.updateComponent.fieldId = 17;
  serverOp.updateComponent.fieldValue = &fieldValue;

  GString* output = g_string_new("");
  bool serialized = shovelerServerOpSerialize(&serverOp, componentTypeIndexer, output);
  ASSERT_FALSE(serialized);
  g_string_free(output, /* freeSegment */ true);
}

TEST_F(ShovelerServerOpTest, FailDeserializeInvalidString) {
  ShovelerServerOpWithData deserializedServerOp;
  shovelerServerOpInitWithData(&deserializedServerOp, /* inputServerOp */ nullptr);
  const char* invalidInput = "the bird is the word";
  int readIndex = 0;
  bool deserialized = shovelerServerOpDeserialize(
      &deserializedServerOp,
      componentTypeIndexer,
      (unsigned char*) invalidInput,
      (int) strlen(invalidInput),
      &readIndex);
  ASSERT_FALSE(deserialized);
  shovelerServerOpClearWithData(&deserializedServerOp);
}
