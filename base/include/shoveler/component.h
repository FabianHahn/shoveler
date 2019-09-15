#ifndef SHOVELER_COMPONENT_H
#define SHOVELER_COMPONENT_H

#include <assert.h> // assert
#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/types.h>

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: below
typedef struct ShovelerComponentConfigurationValueStruct ShovelerComponentConfigurationValue; // forward declaration: below
typedef struct ShovelerComponentTypeConfigurationOptionStruct ShovelerComponentTypeConfigurationOption; // forward declaration: below
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: below
typedef struct ShovelerViewEntityStruct ShovelerViewEntity; // forward declaration: view.h

typedef enum {
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES,
} ShovelerComponentConfigurationOptionType;

typedef struct ShovelerComponentConfigurationValueStruct {
	ShovelerComponentConfigurationOptionType type;
	union {
		long long int entityIdValue;
		struct {
			long long int *entityIds;
			size_t size;
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
			size_t size;
		} bytesValue;
	};
} ShovelerComponentConfigurationValue;

typedef void (ShovelerComponentTypeConfigurationOptionLiveUpdateFunction)(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
typedef void (ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction)(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

typedef struct ShovelerComponentTypeConfigurationOptionStruct {
	char *key;
	ShovelerComponentConfigurationOptionType type;
	bool isOptional;
	ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate;
	ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction *updateDependency;
	char *dependencyComponentTypeName;
} ShovelerComponentTypeConfigurationOption;

typedef void *(ShovelerComponentTypeActivationFunction)(ShovelerComponent *component);
typedef void (ShovelerComponentTypeDeactivationFunction)(ShovelerComponent *component);

typedef struct ShovelerComponentTypeStruct {
	char *name;
	/* map from string configuration option keys to (ShovelerComponentTypeConfigurationOption *) */
	GHashTable *configurationOptions;
	ShovelerComponentTypeActivationFunction *activate;
	ShovelerComponentTypeDeactivationFunction *deactivate;
	bool requiresAuthority;
} ShovelerComponentType;

typedef struct ShovelerComponentStruct {
	ShovelerViewEntity *entity;
	ShovelerComponentType *type;
	/* map from string configuration option keys to (ShovelerComponentConfigurationValue *) */
	GHashTable *configurationValues;
	void *data;
} ShovelerComponent;

ShovelerComponentType *shovelerComponentTypeCreate(const char *name, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate, bool requiresAuthority);
/**
 * Adds a new configuration option to the component type.
 *
 * If the passed live update function is not NULL, this indicates that the configuration option can
 * be updated by calling that function instead of deactivating and reactivating the component.
 */
bool shovelerComponentTypeAddConfigurationOption(ShovelerComponentType *componentType, const char *key, ShovelerComponentConfigurationOptionType type, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate);
/**
 * Adds a new dependency configuration option to the component type.
 *
 * A dependency configuration option is an option of type entity id that specifies a dependency
 * component type name. Components of this type will only be able to activate if the specified
 * entity ID contains an activated component of the specified type.
 *
 * If the passed live update function is not NULL, this indicates that the configuration option can
 * be updated by calling that function instead of deactivating and reactivating the component.
 *
 * If the component is active and the passed update dependency function is not NULL, it will be
 * called whenever the dependency this component value points to is live updated. This includes
 * updates to the dependency's transitive dependencies.
 */
bool shovelerComponentTypeAddDependencyConfigurationOption(ShovelerComponentType *componentType, const char *key, const char *dependencyComponentTypeName, bool isArray, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate, ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction *updateDependency);
bool shovelerComponentTypeRemoveConfigurationOption(ShovelerComponentType *componentType, const char *key);
void shovelerComponentTypeFree(ShovelerComponentType *componentType);

ShovelerComponent *shovelerComponentCreate(ShovelerViewEntity *entity, const char *componentTypeName);
/**
 * Updates a configuration option with the specified key on this component.
 *
 * If value is NULL, resets the configuration option to its default value, or unsets it if it is
 * optional.
 *
 * If isCanonical is true, no authority check is performed and the update is applied without
 * invoking the authoritative update handler.
 */
bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, const char *key, const ShovelerComponentConfigurationValue *value, bool isCanonical);
const ShovelerComponentConfigurationValue *shovelerComponentGetConfigurationValue(ShovelerComponent *component, const char *key);
/**
 * Activates this component if the activation conditions are satisified.
 *
 * To activate the component, the activation function is called and the returned data is stored
 * within the component's data field.
 *
 * Activation can fail under the following conditions:
 *  - The component type requires authority, but the component isn't delegated.
 *  - A dependency configuration option is set but the dependency component doesn't exist or
 *    isn't active itself.
 *  - The component's activation function failed.
 */
bool shovelerComponentActivate(ShovelerComponent *component);
bool shovelerComponentIsActive(ShovelerComponent *component);
/**
 * Deactivates this component if it is active.
 *
 * If any other components depend on this component, they are recursively deactivated before
 * this component is deactivated by calling its deactivation function. After deactivation, the
 * component's data field is set to NULL until the component is activated again.
 */
void shovelerComponentDeactivate(ShovelerComponent *component);
void shovelerComponentFree(ShovelerComponent *component);

/**
 * A ShovelerComponentTypeConfigurationOptionLiveUpdateFunction that does nothing and can be
 * passed to shovelerComponentTypeAddConfigurationOption.
 */
static inline void shovelerComponentLiveUpdateNoop(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	// deliberately do nothing
}

static inline bool shovelerComponentClearConfigurationOption(ShovelerComponent *component, const char *key)
{
	return shovelerComponentUpdateConfigurationOption(component, key, NULL, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionEntityId(ShovelerComponent *component, const char *key, long long int entityIdValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	value.entityIdValue = entityIdValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionEntityIdArray(ShovelerComponent *component, const char *key, const long long int *entityIds, size_t size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY;
	value.entityIdArrayValue.entityIds = (long long int *) entityIds; // won't be modified
	value.entityIdArrayValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionFloat(ShovelerComponent *component, const char *key, float floatValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT;
	value.floatValue = floatValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionBool(ShovelerComponent *component, const char *key, bool boolValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL;
	value.boolValue = boolValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionInt(ShovelerComponent *component, const char *key, int intValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT;
	value.intValue = intValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionUint(ShovelerComponent *component, const char *key, unsigned int uintValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT;
	value.uintValue = uintValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionString(ShovelerComponent *component, const char *key, const char *stringValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING;
	value.stringValue = (char *) stringValue; // won't be modified
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector2(ShovelerComponent *component, const char *key, ShovelerVector2 vector2Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2;
	value.vector2Value = vector2Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector3(ShovelerComponent *component, const char *key, ShovelerVector3 vector3Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3;
	value.vector3Value = vector3Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector4(ShovelerComponent *component, const char *key, ShovelerVector4 vector4Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4;
	value.vector4Value = vector4Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionBytes(ShovelerComponent *component, const char *key, const unsigned char *data, size_t size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES;
	value.bytesValue.data = (unsigned char *) data; // won't be modified
	value.bytesValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentClearCanonicalConfigurationOption(ShovelerComponent *component, const char *key)
{
	return shovelerComponentUpdateConfigurationOption(component, key, NULL, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionEntityId(ShovelerComponent *component, const char *key, long long int entityIdValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	value.entityIdValue = entityIdValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(ShovelerComponent *component, const char *key, const long long int *entityIds, size_t size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY;
	value.entityIdArrayValue.entityIds = (long long int *) entityIds; // won't be modified
	value.entityIdArrayValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionFloat(ShovelerComponent *component, const char *key, float floatValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT;
	value.floatValue = floatValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionBool(ShovelerComponent *component, const char *key, bool boolValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL;
	value.boolValue = boolValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionInt(ShovelerComponent *component, const char *key, int intValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT;
	value.intValue = intValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionUint(ShovelerComponent *component, const char *key, unsigned int uintValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT;
	value.uintValue = uintValue;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionString(ShovelerComponent *component, const char *key, const char *stringValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING;
	value.stringValue = (char *) stringValue; // won't be modified
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionVector2(ShovelerComponent *component, const char *key, ShovelerVector2 vector2Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2;
	value.vector2Value = vector2Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionVector3(ShovelerComponent *component, const char *key, ShovelerVector3 vector3Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3;
	value.vector3Value = vector3Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionVector4(ShovelerComponent *component, const char *key, ShovelerVector4 vector4Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4;
	value.vector4Value = vector4Value;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionBytes(ShovelerComponent *component, const char *key, const unsigned char *data, size_t size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES;
	value.bytesValue.data = (unsigned char *) data; // won't be modified
	value.bytesValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, key, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentHasConfigurationValue(ShovelerComponent *component, const char *key)
{
	return shovelerComponentGetConfigurationValue(component, key) != NULL;
}

static inline long long int shovelerComponentGetConfigurationValueEntityId(ShovelerComponent *component, const char *key)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID);

	return configurationValue->entityIdValue;
}

static inline void shovelerComponentGetConfigurationValueEntityIdArray(ShovelerComponent *component, const char *key, const long long int **outputEntityIds, size_t *outputSize)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY);

	*outputEntityIds = configurationValue->entityIdArrayValue.entityIds;
	*outputSize = configurationValue->entityIdArrayValue.size;
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

static inline void shovelerComponentGetConfigurationValueBytes(ShovelerComponent *component, const char *key, const unsigned char **outputData, size_t *outputSize)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, key);
	assert(configurationValue != NULL);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES);

	*outputData = configurationValue->bytesValue.data;
	*outputSize = configurationValue->bytesValue.size;
}

#endif
