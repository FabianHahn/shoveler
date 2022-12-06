#ifndef SHOVELER_COMPONENT_FIELD_H
#define SHOVELER_COMPONENT_FIELD_H

#include <glib.h>
#include <shoveler/types.h>
#include <stdbool.h> // bool

typedef enum {
  SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID,
  SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY,
  SHOVELER_COMPONENT_FIELD_TYPE_FLOAT,
  SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
  SHOVELER_COMPONENT_FIELD_TYPE_INT,
  SHOVELER_COMPONENT_FIELD_TYPE_STRING,
  SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2,
  SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3,
  SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4,
  SHOVELER_COMPONENT_FIELD_TYPE_BYTES,
} ShovelerComponentFieldType;

typedef struct ShovelerComponentFieldValueStruct {
  ShovelerComponentFieldType type;
  bool isSet;
  union {
    long long int entityIdValue;
    struct {
      long long int* entityIds;
      int size;
    } entityIdArrayValue;
    float floatValue;
    bool boolValue;
    int intValue;
    // string value is owned by the struct itself
    char* stringValue;
    ShovelerVector2 vector2Value;
    ShovelerVector3 vector3Value;
    ShovelerVector4 vector4Value;
    struct {
      unsigned char* data;
      int size;
    } bytesValue;
  };
} ShovelerComponentFieldValue;

typedef struct ShovelerComponentFieldStruct {
  const char* name;
  ShovelerComponentFieldType type;
  bool isOptional;
  const char* dependencyComponentTypeId;
} ShovelerComponentField;

/**
 * Constructor for a value component field.
 */
ShovelerComponentField shovelerComponentField(
    const char* name, ShovelerComponentFieldType type, bool isOptional);

/**
 * Constructor for a dependency component field.
 *
 * A dependency field is a field of type entity id that specifies a dependency
 * component type.
 */
ShovelerComponentField shovelerComponentFieldDependency(
    const char* name, const char* dependencyComponentTypeId, bool isArray, bool isOptional);

ShovelerComponentFieldValue* shovelerComponentFieldCreateValue(ShovelerComponentFieldType type);
void shovelerComponentFieldInitValue(
    ShovelerComponentFieldValue* fieldValue, ShovelerComponentFieldType type);
GString* shovelerComponentFieldPrintValue(const ShovelerComponentFieldValue* fieldValue);
ShovelerComponentFieldValue* shovelerComponentFieldCopyValue(
    const ShovelerComponentFieldValue* reference);
void shovelerComponentFieldClearValue(ShovelerComponentFieldValue* fieldValue);
void shovelerComponentFieldAssignValue(
    ShovelerComponentFieldValue* target, const ShovelerComponentFieldValue* source);
bool shovelerComponentFieldCompareValue(
    const ShovelerComponentFieldValue* a, const ShovelerComponentFieldValue* b);
/** Appends the serialized representation of the field value to the passed string. */
bool shovelerComponentFieldSerializeValue(
    const ShovelerComponentFieldValue* fieldValue, GString* output);
/**
 * Deserializes from the passed buffer starting at readIndex, incrementing readIndex as bytes are
 * read.
 */
bool shovelerComponentFieldDeserializeValue(
    ShovelerComponentFieldValue* fieldValue,
    const unsigned char* buffer,
    int bufferSize,
    int* readIndex);
void shovelerComponentFieldFreeValue(ShovelerComponentFieldValue* fieldValue);

#endif
