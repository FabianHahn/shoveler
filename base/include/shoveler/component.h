#ifndef SHOVELER_COMPONENT_H
#define SHOVELER_COMPONENT_H

#include <assert.h> // assert
#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/types.h>

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: below
typedef struct ShovelerComponentTypeConfigurationOptionStruct ShovelerComponentTypeConfigurationOption; // forward declaration: below
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: below

typedef enum {
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4,
} ShovelerComponentConfigurationOptionType;

typedef	struct {
	ShovelerComponentConfigurationOptionType type;
	union {
		long long int entityIdValue;
		float floatValue;
		bool boolValue;
		int intValue;
		unsigned int uintValue;
		// string value is owned by the struct itself
		char *stringValue;
		ShovelerVector2 vector2Value;
		ShovelerVector3 vector3Value;
		ShovelerVector4 vector4Value;
	};
} ShovelerComponentConfigurationValue;

typedef void (ShovelerComponentTypeConfigurationOptionUpdateFunction)(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *userData);

typedef struct ShovelerComponentTypeConfigurationOptionStruct {
	char *key;
	ShovelerComponentConfigurationOptionType type;
	ShovelerComponentConfigurationValue *defaultValue;
	ShovelerComponentTypeConfigurationOptionUpdateFunction *update;
} ShovelerComponentTypeConfigurationOption;

typedef void *(ShovelerComponentTypeActivationFunction)(ShovelerComponent *component, void *userData);
typedef void (ShovelerComponentTypeDeactivationFunction)(ShovelerComponent *component, void *userData);

typedef struct ShovelerComponentTypeStruct {
	char *name;
	/* map from string configuration option keys to (ShovelerComponentConfiguration *) */
	GHashTable *configurationOptions;
	ShovelerComponentTypeActivationFunction *activate;
	ShovelerComponentTypeDeactivationFunction *deactivate;
} ShovelerComponentType;

typedef struct ShovelerComponentStruct {
	ShovelerComponentType *type;
	/* map from string configuration option keys to (ShovelerComponentConfigurationValue *) */
	GHashTable *configurationValues;
	void *data;
	void *callbackUserData;
} ShovelerComponent;

ShovelerComponentType *shovelerComponentTypeCreate(const char *name, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate);
/**
 * Adds a new configuration option to the component type.
 *
 * If the passed update function is not NULL, this indicates that the configuration option can be
 * updated without deactivating and reactivating the component.
 */
bool shovelerComponentTypeAddConfigurationOption(ShovelerComponentType *componentType, const char *key, ShovelerComponentConfigurationOptionType type, const ShovelerComponentConfigurationValue *defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update);
bool shovelerComponentTypeRemoveConfigurationOption(ShovelerComponentType *componentType, const char *key);
void shovelerComponentTypeFree(ShovelerComponentType *componentType);

static inline bool shovelerComponentTypeAddConfigurationOptionEntityId(ShovelerComponentType *componentType, const char *key, long long int defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	value.entityIdValue = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionFloat(ShovelerComponentType *componentType, const char *key, float defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT;
	value.floatValue = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionBool(ShovelerComponentType *componentType, const char *key, bool defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL;
	value.boolValue = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionInt(ShovelerComponentType *componentType, const char *key, int defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT;
	value.intValue = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionUint(ShovelerComponentType *componentType, const char *key, unsigned int defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT;
	value.uintValue = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionString(ShovelerComponentType *componentType, const char *key, const char *defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING;
	value.stringValue = (char *) defaultValue; // won't be modified
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionVector2(ShovelerComponentType *componentType, const char *key, ShovelerVector2 defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2;
	value.vector2Value = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionVector3(ShovelerComponentType *componentType, const char *key, ShovelerVector3 defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3;
	value.vector3Value = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

static inline bool shovelerComponentTypeAddConfigurationOptionVector4(ShovelerComponentType *componentType, const char *key, ShovelerVector4 defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4;
	value.vector4Value = defaultValue;
	return shovelerComponentTypeAddConfigurationOption(componentType, key, value.type, &value, update);
}

ShovelerComponent *shovelerComponentCreate(ShovelerComponentType *type, void *callbackUserData);
bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, const char *key, const ShovelerComponentConfigurationValue *value);
const ShovelerComponentConfigurationValue *shovelerComponentGetConfigurationValue(ShovelerComponent *component, const char *key);
bool shovelerComponentActivate(ShovelerComponent *component);
bool shovelerComponentIsActive(ShovelerComponent *component);
bool shovelerComponentDeactivate(ShovelerComponent *component);
void shovelerComponentFree(ShovelerComponent *component);

static inline bool shovelerComponentUpdateConfigurationOptionEntityId(ShovelerComponent *component, const char *key, long long int entityIdValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	value.entityIdValue = entityIdValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionFloat(ShovelerComponent *component, const char *key, float floatValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT;
	value.floatValue = floatValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionBool(ShovelerComponent *component, const char *key, bool boolValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL;
	value.boolValue = boolValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionInt(ShovelerComponent *component, const char *key, int intValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT;
	value.intValue = intValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionUint(ShovelerComponent *component, const char *key, unsigned int uintValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT;
	value.uintValue = uintValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionString(ShovelerComponent *component, const char *key, const char *stringValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING;
	value.stringValue = (char *) stringValue; // won't be modified
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector2(ShovelerComponent *component, const char *key, ShovelerVector2 vector2Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2;
	value.vector2Value = vector2Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector3(ShovelerComponent *component, const char *key, ShovelerVector3 vector3Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3;
	value.vector3Value = vector3Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector4(ShovelerComponent *component, const char *key, ShovelerVector4 vector4Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4;
	value.vector4Value = vector4Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value);
}

static inline long long int shovelerComponentGetConfigurationValueEntityId(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID);

	return configurationValue->entityIdValue;
}

static inline float shovelerComponentGetConfigurationValueFloat(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT);

	return configurationValue->floatValue;
}

static inline bool shovelerComponentGetConfigurationValueBool(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL);

	return configurationValue->boolValue;
}

static inline int shovelerComponentGetConfigurationValueInt(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT);

	return configurationValue->intValue;
}

static inline unsigned int shovelerComponentGetConfigurationValueUint(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT);

	return configurationValue->uintValue;
}

static inline const char *shovelerComponentGetConfigurationValueString(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING);

	return configurationValue->stringValue;
}

static inline ShovelerVector2 shovelerComponentGetConfigurationValueVector2(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2);

	return configurationValue->vector2Value;
}

static inline ShovelerVector3 shovelerComponentGetConfigurationValueVector3(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3);

	return configurationValue->vector3Value;
}

static inline ShovelerVector4 shovelerComponentGetConfigurationValueVector4(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4);

	return configurationValue->vector4Value;
}

#endif