#include "shoveler/component_field.h"

#include <assert.h> // assert
#include <stdlib.h> // NULL, malloc, free
#include <string.h> // memset

ShovelerComponentField shovelerComponentField(const char *name, ShovelerComponentFieldType type, bool isOptional)
{
	ShovelerComponentField field;
	field.name = name;
	field.type = type;
	field.isOptional = isOptional;
	field.dependencyComponentTypeId = NULL;

	return field;
}

ShovelerComponentField shovelerComponentFieldDependency(const char *name, const char *dependencyComponentTypeId, bool isArray, bool isOptional)
{
	ShovelerComponentField field;
	field.name = name;
	field.type = isArray ? SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY : SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID;
	field.isOptional = isOptional;
	field.dependencyComponentTypeId = dependencyComponentTypeId;

	return field;
}

ShovelerComponentFieldValue *shovelerComponentFieldCreateValue(ShovelerComponentFieldType type)
{
	ShovelerComponentFieldValue *fieldValue = malloc(sizeof(ShovelerComponentFieldValue));
	shovelerComponentFieldInitValue(fieldValue, type);
	return fieldValue;
}

void shovelerComponentFieldInitValue(ShovelerComponentFieldValue *fieldValue, ShovelerComponentFieldType type)
{
	memset(fieldValue, 0, sizeof(ShovelerComponentFieldValue));
	fieldValue->type = type;
	fieldValue->isSet = false;
}

GString *shovelerComponentFieldPrintValue(const ShovelerComponentFieldValue *fieldValue)
{
	GString *printed = g_string_new("");

	switch(fieldValue->type) {
		case SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID:
			g_string_append_printf(printed, "%lld", fieldValue->entityIdValue);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY:
			g_string_append(printed, "[");
			for(int i = 0; i < fieldValue->entityIdArrayValue.size; i++) {
				if(i > 0) {
					g_string_append(printed, ", ");
				}

				g_string_append_printf(printed, "%lld", fieldValue->entityIdValue);
			}
			g_string_append(printed, "]");
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_FLOAT:
			g_string_append_printf(printed, "%.3f", fieldValue->floatValue);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_BOOL:
			g_string_append(printed, fieldValue->boolValue ? "true" : "false");
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_INT:
			g_string_append_printf(printed, "%d", fieldValue->intValue);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_STRING:
			g_string_append(printed, fieldValue->stringValue);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2:
			g_string_append_printf(printed, "(%.3f, %.3f)", fieldValue->vector2Value.values[0], fieldValue->vector2Value.values[1]);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3:
			g_string_append_printf(printed, "(%.3f, %.3f, %.3f)", fieldValue->vector3Value.values[0], fieldValue->vector3Value.values[1], fieldValue->vector3Value.values[2]);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4:
			g_string_append_printf(printed, "(%.3f, %.3f, %.3f, %.3f)", fieldValue->vector4Value.values[0], fieldValue->vector4Value.values[1], fieldValue->vector4Value.values[2], fieldValue->vector4Value.values[3]);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_BYTES:
			g_string_append_printf(printed, "%d bytes", fieldValue->bytesValue.size);
			break;
	}

	return printed;
}

ShovelerComponentFieldValue *shovelerComponentFieldCopyValue(const ShovelerComponentFieldValue *reference)
{
	ShovelerComponentFieldValue *copy = shovelerComponentFieldCreateValue(reference->type);
	shovelerComponentFieldAssignValue(copy, reference);
	return copy;
}

void shovelerComponentFieldClearValue(ShovelerComponentFieldValue *fieldValue)
{
	fieldValue->isSet = false;

	switch(fieldValue->type) {
		case SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID:
			fieldValue->entityIdValue = 0;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY:
			free(fieldValue->entityIdArrayValue.entityIds);
			fieldValue->entityIdArrayValue.entityIds = NULL;
			fieldValue->entityIdArrayValue.size = 0;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_FLOAT:
			fieldValue->floatValue = 0.0f;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_BOOL:
			fieldValue->boolValue = false;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_INT:
			fieldValue->intValue = 0;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_STRING:
			free(fieldValue->stringValue);
			fieldValue->stringValue = NULL;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2:
			fieldValue->vector2Value = shovelerVector2(0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3:
			fieldValue->vector3Value = shovelerVector3(0.0f, 0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4:
			fieldValue->vector4Value = shovelerVector4(0.0f, 0.0f, 0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_BYTES:
			free(fieldValue->bytesValue.data);
			fieldValue->bytesValue.data = NULL;
			fieldValue->bytesValue.size = 0;
			break;
	}
}

void shovelerComponentFieldAssignValue(ShovelerComponentFieldValue *target, const ShovelerComponentFieldValue *source)
{
	assert(source != NULL);
	assert(target != NULL);
	assert(target->type == source->type);

	// no self assignment
	assert(source != target);
	assert(source->type != SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY || (source->entityIdArrayValue.entityIds != target->entityIdArrayValue.entityIds));
	assert(source->type != SHOVELER_COMPONENT_FIELD_TYPE_STRING || (source->stringValue != target->stringValue));
	assert(source->type != SHOVELER_COMPONENT_FIELD_TYPE_BYTES || (source->bytesValue.data != target->bytesValue.data));

	shovelerComponentFieldClearValue(target);

	target->isSet = source->isSet;
	if(!target->isSet) {
		return;
	}

	switch(target->type) {
		case SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID:
			target->entityIdValue = source->entityIdValue;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_ENTITY_ID_ARRAY: {
			if(source->entityIdArrayValue.size > 0) {
				size_t numElements = (size_t) source->entityIdArrayValue.size;

				target->entityIdArrayValue.entityIds = malloc(numElements * sizeof(long long int));
				target->entityIdArrayValue.size = source->entityIdArrayValue.size;
				memcpy(target->entityIdArrayValue.entityIds, source->entityIdArrayValue.entityIds, numElements * sizeof(long long int));
			}
		} break;
		case SHOVELER_COMPONENT_FIELD_TYPE_FLOAT:
			target->floatValue = source->floatValue;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_BOOL:
			target->boolValue = source->boolValue;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_INT:
			target->intValue = source->intValue;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_STRING:
			target->stringValue = strdup(source->stringValue);
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2:
			target->vector2Value = source->vector2Value;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3:
			target->vector3Value = source->vector3Value;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4:
			target->vector4Value = source->vector4Value;
			break;
		case SHOVELER_COMPONENT_FIELD_TYPE_BYTES: {
			if(source->entityIdArrayValue.size > 0) {
				size_t numBytes = (size_t) source->bytesValue.size;

				target->bytesValue.data = malloc(numBytes * sizeof(unsigned char));
				target->bytesValue.size = source->bytesValue.size;
				memcpy(target->bytesValue.data, source->bytesValue.data, numBytes * sizeof(unsigned char));
			}
		} break;
	}
}

void shovelerComponentFieldFreeValue(ShovelerComponentFieldValue *fieldValue)
{
	shovelerComponentFieldClearValue(fieldValue);
	free(fieldValue);
}
