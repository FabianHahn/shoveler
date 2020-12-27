#include "shoveler/component_data.h"

#include <assert.h> // assert
#include <stdlib.h> // NULL, malloc, free
#include <string.h> // memset

ShovelerComponentDataField shovelerComponentDataField(const char *name, ShovelerComponentDataFieldType type, bool isOptional)
{
	ShovelerComponentDataField field;
	field.name = name;
	field.type = type;
	field.isOptional = isOptional;
	field.dependencyComponentTypeId = NULL;

	return field;
}

ShovelerComponentDataField shovelerComponentDataFieldDependency(const char *name, const char *dependencyComponentTypeId, bool isArray, bool isOptional)
{
	ShovelerComponentDataField field;
	field.name = name;
	field.type = isArray ? SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID_ARRAY : SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID;
	field.isOptional = isOptional;
	field.dependencyComponentTypeId = dependencyComponentTypeId;

	return field;
}

ShovelerComponentDataFieldValue *shovelerComponentDataCreateFieldValue(ShovelerComponentDataFieldType type)
{
	ShovelerComponentDataFieldValue *fieldValue = malloc(sizeof(ShovelerComponentDataFieldValue));
	shovelerComponentDataInitFieldValue(fieldValue, type);
	return fieldValue;
}

void shovelerComponentDataInitFieldValue(ShovelerComponentDataFieldValue *fieldValue, ShovelerComponentDataFieldType type)
{
	memset(fieldValue, 0, sizeof(ShovelerComponentDataFieldValue));
	fieldValue->type = type;
	fieldValue->isSet = false;
}

ShovelerComponentDataFieldValue *shovelerComponentDataCopyFieldValue(const ShovelerComponentDataFieldValue *reference)
{
	ShovelerComponentDataFieldValue *copy = shovelerComponentDataCreateFieldValue(reference->type);
	shovelerComponentDataAssignFieldValue(copy, reference);
	return copy;
}

void shovelerComponentDataClearFieldValue(ShovelerComponentDataFieldValue *fieldValue)
{
	fieldValue->isSet = false;

	switch(fieldValue->type) {
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID:
			fieldValue->entityIdValue = 0;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID_ARRAY:
			free(fieldValue->entityIdArrayValue.entityIds);
			fieldValue->entityIdArrayValue.entityIds = NULL;
			fieldValue->entityIdArrayValue.size = 0;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_FLOAT:
			fieldValue->floatValue = 0.0f;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_BOOL:
			fieldValue->boolValue = false;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_INT:
			fieldValue->intValue = 0;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_STRING:
			free(fieldValue->stringValue);
			fieldValue->stringValue = NULL;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR2:
			fieldValue->vector2Value = shovelerVector2(0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR3:
			fieldValue->vector3Value = shovelerVector3(0.0f, 0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR4:
			fieldValue->vector4Value = shovelerVector4(0.0f, 0.0f, 0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_BYTES:
			free(fieldValue->bytesValue.data);
			fieldValue->bytesValue.data = NULL;
			fieldValue->bytesValue.size = 0;
			break;
	}
}

void shovelerComponentDataAssignFieldValue(ShovelerComponentDataFieldValue *target, const ShovelerComponentDataFieldValue *source)
{
	assert(source != NULL);
	assert(target != NULL);
	assert(target->type == source->type);

	// no self assignment
	assert(source != target);
	assert(source->type != SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID_ARRAY || (source->entityIdArrayValue.entityIds != target->entityIdArrayValue.entityIds));
	assert(source->type != SHOVELER_COMPONENT_DATA_FIELD_TYPE_STRING || (source->stringValue != target->stringValue));
	assert(source->type != SHOVELER_COMPONENT_DATA_FIELD_TYPE_BYTES || (source->bytesValue.data != target->bytesValue.data));

	shovelerComponentDataClearFieldValue(target);

	target->isSet = source->isSet;
	if(!target->isSet) {
		return;
	}

	switch(target->type) {
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID:
			target->entityIdValue = source->entityIdValue;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_ENTITY_ID_ARRAY: {
			if(source->entityIdArrayValue.size > 0) {
				size_t numElements = (size_t) source->entityIdArrayValue.size;

				target->entityIdArrayValue.entityIds = malloc(numElements * sizeof(long long int));
				target->entityIdArrayValue.size = source->entityIdArrayValue.size;
				memcpy(target->entityIdArrayValue.entityIds, source->entityIdArrayValue.entityIds, numElements * sizeof(long long int));
			}
		} break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_FLOAT:
			target->floatValue = source->floatValue;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_BOOL:
			target->boolValue = source->boolValue;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_INT:
			target->intValue = source->intValue;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_STRING:
			target->stringValue = strdup(source->stringValue);
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR2:
			target->vector2Value = source->vector2Value;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR3:
			target->vector3Value = source->vector3Value;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_VECTOR4:
			target->vector4Value = source->vector4Value;
			break;
		case SHOVELER_COMPONENT_DATA_FIELD_TYPE_BYTES: {
			if(source->entityIdArrayValue.size > 0) {
				size_t numBytes = (size_t) source->bytesValue.size;

				target->bytesValue.data = malloc(numBytes * sizeof(unsigned char));
				target->bytesValue.size = source->bytesValue.size;
				memcpy(target->bytesValue.data, source->bytesValue.data, numBytes * sizeof(unsigned char));
			}
		} break;
	}
}

void shovelerComponentDataFreeFieldValue(ShovelerComponentDataFieldValue *fieldValue)
{
	shovelerComponentDataClearFieldValue(fieldValue);
	free(fieldValue);
}
