#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "shoveler/client_op.h"
#include "shoveler/component_type_indexer.h"
}

namespace {
const char* testComponentType1 = "component_type_1";
const char* testComponentType2 = "component_type_2";
const char* testComponentType3 = "component_type_3";

class ShovelerClientOpTest : public ::testing::Test {
public:
  virtual void SetUp() {
    componentTypeIndexer = shovelerComponentTypeIndexerCreate();
    shovelerComponentTypeIndexerAddComponentType(componentTypeIndexer, testComponentType3);
    shovelerComponentTypeIndexerAddComponentType(componentTypeIndexer, testComponentType1);
  }

  virtual void TearDown() { shovelerComponentTypeIndexerFree(componentTypeIndexer); }

  void TestSerialization(const ShovelerClientOp* clientOp) {
    GString* output = g_string_new("");
    bool serialized = shovelerClientOpSerialize(clientOp, componentTypeIndexer, output);
    ASSERT_TRUE(serialized);
    ASSERT_GT(output->len, 0);

    ShovelerClientOpWithData deserializedClientOp;
    shovelerClientOpInitWithData(&deserializedClientOp, /* inputClientOp */ nullptr);
    int readIndex = 0;
    bool deserialized = shovelerClientOpDeserialize(
        &deserializedClientOp,
        componentTypeIndexer,
        (unsigned char*) output->str,
        (int) output->len,
        &readIndex);
    ASSERT_TRUE(deserialized);
    ASSERT_EQ(readIndex, output->len);
    ASSERT_TRUE(shovelerClientOpEquals(clientOp, &deserializedClientOp.op));

    shovelerClientOpClearWithData(&deserializedClientOp);
    g_string_free(output, /* freeSegment */ true);
  }

  ShovelerComponentTypeIndexer* componentTypeIndexer;
};

} // namespace

TEST_F(ShovelerClientOpTest, SerializeAddEntity) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_ADD_ENTITY;
  clientOp.addEntity.entityId = 42;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeRemoveEntity) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_REMOVE_ENTITY;
  clientOp.removeEntity.entityId = 9001;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeAddComponent) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_ADD_COMPONENT;
  clientOp.addComponent.entityId = 42;
  clientOp.addComponent.componentTypeId = testComponentType1;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeUpdateComponent) {
  ShovelerComponentFieldValue fieldValue;
  fieldValue.type = SHOVELER_COMPONENT_FIELD_TYPE_STRING;
  fieldValue.isSet = true;
  fieldValue.stringValue = (char*) "the bird is the word"; // not modified

  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_UPDATE_COMPONENT;
  clientOp.updateComponent.entityId = 42;
  clientOp.updateComponent.componentTypeId = testComponentType3;
  clientOp.updateComponent.fieldId = 17;
  clientOp.updateComponent.fieldValue = &fieldValue;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeActivateComponent) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT;
  clientOp.activateComponent.entityId = 99999999999;
  clientOp.activateComponent.componentTypeId = testComponentType3;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeDeactivateComponent) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT;
  clientOp.deactivateComponent.entityId = 99999999999;
  clientOp.deactivateComponent.componentTypeId = testComponentType1;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeDelegateComponent) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_DELEGATE_COMPONENT;
  clientOp.delegateComponent.entityId = 0;
  clientOp.delegateComponent.componentTypeId = testComponentType1;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeUndelegateComponent) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT;
  clientOp.undelegateComponent.entityId = 99999999999;
  clientOp.undelegateComponent.componentTypeId = testComponentType3;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, SerializeRemoveComponent) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_REMOVE_COMPONENT;
  clientOp.removeComponent.entityId = 9001;
  clientOp.removeComponent.componentTypeId = testComponentType3;

  ASSERT_NO_FATAL_FAILURE(TestSerialization(&clientOp));
}

TEST_F(ShovelerClientOpTest, FailSerializeInvalidComponentType) {
  ShovelerClientOp clientOp;
  clientOp.type = SHOVELER_CLIENT_OP_ADD_COMPONENT;
  clientOp.addComponent.entityId = 42;
  clientOp.addComponent.componentTypeId = testComponentType2;

  GString* output = g_string_new("");
  bool serialized = shovelerClientOpSerialize(&clientOp, componentTypeIndexer, output);
  ASSERT_FALSE(serialized);
  g_string_free(output, /* freeSegment */ true);
}

TEST_F(ShovelerClientOpTest, FailDeserializeInvalidString) {
  ShovelerClientOpWithData deserializedClientOp;
  shovelerClientOpInitWithData(&deserializedClientOp, /* inputClientOp */ nullptr);
  const char* invalidInput = "the bird is the word";
  int readIndex = 0;
  bool deserialized = shovelerClientOpDeserialize(
      &deserializedClientOp,
      componentTypeIndexer,
      (unsigned char*) invalidInput,
      (int) strlen(invalidInput),
      &readIndex);
  ASSERT_FALSE(deserialized);
  shovelerClientOpClearWithData(&deserializedClientOp);
}
