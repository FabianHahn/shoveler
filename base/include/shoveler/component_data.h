#ifndef SHOVELER_COMPONENT_DATA_H
#define SHOVELER_COMPONENT_DATA_H

#include <stdbool.h> // bool

#include <shoveler/types.h>

typedef enum {
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID_ARRAY,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_FLOAT,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_BOOL,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_INT,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_STRING,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR2,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR3,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR4,
	SHOVELER_COMPONENT_DATA_FIELD_TYPE_BYTES,
} ShovelerComponentDataFieldType;

typedef struct ShovelerComponentDataFieldValueStruct {
	ShovelerComponentDataFieldType type;
	bool isSet;
	union {
		long long int entityIdValue;
		struct {
			long long int *entityIds;
			int size;
		} entityIdArrayValue;
		float floatValue;
		bool boolValue;
		int intValue;
		unsigned int uintValue;
		// string value is owned by the struct itself
		char *stringValue;
		ShovelerVector2 vector2Value;
		ShovelerVector3 vector3Value;
		ShovelerVector4 vector4Value;
		struct {
			unsigned char *data;
			int size;
		} bytesValue;
	};
} ShovelerComponentDataFieldValue;

typedef struct ShovelerComponentDataFieldStruct {
	const char *name;
	ShovelerComponentDataFieldType type;
	bool isOptional;
	const char *dependencyComponentTypeId;
} ShovelerComponentDataField;

/**
 * Constructor for a value component data field.
 */
ShovelerComponentDataField shovelerComponentDataField(const char *name, ShovelerComponentDataFieldType type, bool isOptional);

/**
 * Constructor for a dependency component data field.
 *
 * A dependency configuration option is an option of type entity id that specifies a dependency
 * component type name. Components of this type will only be able to activate if the specified
 * entity ID contains an activated component of the specified type.
 */
ShovelerComponentDataField shovelerComponentDataFieldDependency(const char *name, const char *dependencyComponentTypeId, bool isArray, bool isOptional);

ShovelerComponentDataFieldValue *shovelerComponentDataCreateFieldValue(ShovelerComponentDataFieldType type);
void shovelerComponentDataInitFieldValue(ShovelerComponentDataFieldValue *fieldValue, ShovelerComponentDataFieldType type);
ShovelerComponentDataFieldValue *shovelerComponentDataCopyFieldValue(const ShovelerComponentDataFieldValue *reference);
void shovelerComponentDataClearFieldValue(ShovelerComponentDataFieldValue *fieldValue);
void shovelerComponentDataAssignFieldValue(ShovelerComponentDataFieldValue *target, const ShovelerComponentDataFieldValue *source);
void shovelerComponentDataFreeFieldValue(ShovelerComponentDataFieldValue *fieldValue);

#endif
