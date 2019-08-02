#ifndef SHOVELER_COMPONENT_H
#define SHOVELER_COMPONENT_H

#include <stdbool.h>

#include <glib.h>

#include <shoveler/types.h>

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: below
typedef struct ShovelerComponentTypeConfigurationOptionStruct ShovelerComponentTypeConfigurationOption; // forward declaration: below
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: below

typedef enum {
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_ENTITY_ID,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_FLOAT,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_BOOL,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_INT,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_UINT,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_STRING,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_VECTOR2,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_VECTOR3,
	SHOVELER_COMPONENT_TYPE_CONFIGURATION_OPTION_TYPE_VECTOR4,
} ShovelerComponentTypeConfigurationOptionType;

typedef	union {
	long long int entityIdValue;
	float floatValue;
	bool boolValue;
	int intValue;
	unsigned int uintValue;
	char *stringValue;
	ShovelerVector2 vector2Value;
	ShovelerVector3 vector3Value;
	ShovelerVector4 vector4Value;
} ShovelerComponentConfigurationValue;

typedef void (ShovelerComponentTypeConfigurationOptionUpdateFunction)(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponentConfigurationValue previousValue, void *userData);

typedef struct ShovelerComponentTypeConfigurationOptionStruct {
	char *key;
	ShovelerComponentTypeConfigurationOptionType type;
	ShovelerComponentConfigurationValue defaultValue;
	ShovelerComponentTypeConfigurationOptionUpdateFunction *update;
} ShovelerComponentTypeConfigurationOption;

typedef void *(ShovelerComponentTypeActivationFunction)(ShovelerComponent *component, void *userData);
typedef void (ShovelerComponentTypeDeactivationFunction)(ShovelerComponent *component, void *userData);

typedef struct ShovelerComponentTypeStruct {
	char *name;
	void *callbackUserData;
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
} ShovelerComponent;

ShovelerComponentType *shovelerComponentTypeCreate(const char *name, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate, void *callbackUserData);
bool shovelerComponentTypeAddConfigurationOption(ShovelerComponentType *componentType, const char *key, ShovelerComponentTypeConfigurationOptionType type, ShovelerComponentConfigurationValue defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update);
bool shovelerComponentTypeRemoveConfigurationOption(ShovelerComponentType *componentType, const char *key);
void shovelerComponentTypeFree(ShovelerComponentType *componentType);

ShovelerComponent *shovelerComponentCreate(ShovelerComponentType *type);
bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, const char *key, ShovelerComponentConfigurationValue value);
bool shovelerComponentActivate(ShovelerComponent *component);
bool shovelerComponentDeactivate(ShovelerComponent *component);
void shovelerComponentFree(ShovelerComponent *component);

#endif