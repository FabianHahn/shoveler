#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "shoveler/component_field.h"
}

namespace {
class ShovelerComponentFieldTest : public ::testing::Test {
public:
  virtual void SetUp() {}

  virtual void TearDown() { shovelerComponentFieldClearValue(&fieldValue); }

  void TestSerialization() {
    GString* output = g_string_new("");
    bool serialized = shovelerComponentFieldSerializeValue(&fieldValue, output);
    ASSERT_TRUE(serialized);
    ASSERT_GT(output->len, 0);

    ShovelerComponentFieldValue deserializedValue;
    shovelerComponentFieldInitValue(&deserializedValue, SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID);
    int readIndex = 0;
    bool deserialized = shovelerComponentFieldDeserializeValue(
        &deserializedValue, (unsigned char*) output->str, (int) output->len, &readIndex);
    ASSERT_TRUE(deserialized);
    ASSERT_EQ(readIndex, output->len);
    ASSERT_TRUE(shovelerComponentFieldCompareValue(&fieldValue, &deserializedValue));

    shovelerComponentFieldClearValue(&deserializedValue);
    g_string_free(output, /* freeSegment */ true);
  }

  ShovelerComponentFieldValue fieldValue;
};
} // namespace

TEST_F(ShovelerComponentFieldTest, SerializeFieldUnset) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID);

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldEntityId) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID);
  fieldValue.isSet = true;
  fieldValue.entityIdValue = 42;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldEmptyEntityIdArray) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY);
  fieldValue.isSet = true;
  fieldValue.entityIdArrayValue.size = 0;
  fieldValue.entityIdArrayValue.entityIds = nullptr;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldEntityIdArray) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY);
  fieldValue.isSet = true;
  fieldValue.entityIdArrayValue.size = 3;
  fieldValue.entityIdArrayValue.entityIds =
      static_cast<long long int*>(malloc(3 * sizeof(long long int)));
  fieldValue.entityIdArrayValue.entityIds[0] = 42;
  fieldValue.entityIdArrayValue.entityIds[1] = 27;
  fieldValue.entityIdArrayValue.entityIds[2] = 9001;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldFloat) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_FLOAT);
  fieldValue.isSet = true;
  fieldValue.floatValue = 3.14159f;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldBool) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_BOOL);
  fieldValue.isSet = true;
  fieldValue.boolValue = true;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldInt) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_INT);
  fieldValue.isSet = true;
  fieldValue.intValue = 42;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldEmptyString) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_STRING);
  fieldValue.isSet = true;
  fieldValue.stringValue = NULL;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldString) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_STRING);
  fieldValue.isSet = true;
  fieldValue.stringValue = strdup("the bird is the word");

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldVector2) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2);
  fieldValue.isSet = true;
  fieldValue.vector2Value = shovelerVector2(1.0f, 2.0f);

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldVector3) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3);
  fieldValue.isSet = true;
  fieldValue.vector3Value = shovelerVector3(1.0f, 2.0f, 3.0f);

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldVector4) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4);
  fieldValue.isSet = true;
  fieldValue.vector4Value = shovelerVector4(1.0f, 2.0f, 3.0f, 4.0f);

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldEmptyBytes) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_BYTES);
  fieldValue.isSet = true;
  fieldValue.bytesValue.size = 0;
  fieldValue.bytesValue.data = nullptr;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, SerializeFieldBytes) {
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_BYTES);
  fieldValue.isSet = true;
  fieldValue.bytesValue.size = 4;
  fieldValue.bytesValue.data = static_cast<unsigned char*>(malloc(4 * sizeof(unsigned char)));
  fieldValue.bytesValue.data[0] = 0;
  fieldValue.bytesValue.data[1] = 1;
  fieldValue.bytesValue.data[2] = 2;
  fieldValue.bytesValue.data[3] = 3;

  ASSERT_NO_FATAL_FAILURE(TestSerialization());
}

TEST_F(ShovelerComponentFieldTest, FailDeserializeInvalidString) {
  const char* invalidInput = "the bird is the word";
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID);
  int readIndex = 0;
  bool deserialized = shovelerComponentFieldDeserializeValue(
      &fieldValue, (unsigned char*) invalidInput, (int) strlen(invalidInput), &readIndex);
  ASSERT_FALSE(deserialized);
}
