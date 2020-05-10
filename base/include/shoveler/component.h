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
typedef struct ShovelerComponentViewAdapterStruct ShovelerComponentViewAdapter; // forward delcaration: below
typedef struct ShovelerViewEntityStruct ShovelerViewEntity; // forward declaration: view.h

typedef enum {
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4,
	SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES,
} ShovelerComponentConfigurationOptionType;

typedef struct ShovelerComponentConfigurationValueStruct {
	ShovelerComponentConfigurationOptionType type;
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
} ShovelerComponentConfigurationValue;

typedef void (ShovelerComponentTypeConfigurationOptionLiveUpdateFunction)(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
typedef void (ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction)(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

typedef struct ShovelerComponentTypeConfigurationOptionStruct {
	const char *name;
	ShovelerComponentConfigurationOptionType type;
	bool isOptional;
	ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate;
	ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction *updateDependency;
	const char *dependencyComponentTypeId;
} ShovelerComponentTypeConfigurationOption;

typedef void *(ShovelerComponentTypeActivationFunction)(ShovelerComponent *component);
typedef void (ShovelerComponentTypeDeactivationFunction)(ShovelerComponent *component);

typedef struct ShovelerComponentTypeStruct {
	const char *id;
	int numConfigurationOptions;
	ShovelerComponentTypeConfigurationOption *configurationOptions;
	ShovelerComponentTypeActivationFunction *activate;
	ShovelerComponentTypeDeactivationFunction *deactivate;
	bool requiresAuthority;
} ShovelerComponentType;

typedef void (ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction)(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *userData);

typedef ShovelerComponent *(ShovelerComponentViewAdapterGetComponentFunction)(ShovelerComponent *component, long long int entityId, const char *componentTypeId, void *userData);
typedef void (ShovelerComponentViewAdapterUpdateAuthoritativeComponentFunction)(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *userData);
typedef void *(ShovelerComponentViewAdapterGetTargetFunction)(ShovelerComponent *component, const char *targetName, void *userData);
typedef void (ShovelerComponentViewAdapterAddDependencyFunction)(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *userData);
typedef bool (ShovelerComponentViewAdapterRemoveDependencyFunction)(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *userData);
typedef void (ShovelerComponentViewAdapterForEachReverseDependencyFunction)(ShovelerComponent *component, ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *adapterUserData);
typedef void (ShovelerComponentViewAdapterReportActivationFunction)(ShovelerComponent *component, int delta, void *userData);

// Adapter struct to make a component integrate with a view.
typedef struct ShovelerComponentViewAdapterStruct {
	ShovelerComponentViewAdapterGetComponentFunction *getComponent;
	ShovelerComponentViewAdapterUpdateAuthoritativeComponentFunction *updateAuthoritativeComponent;
	ShovelerComponentViewAdapterGetTargetFunction *getTarget;
	ShovelerComponentViewAdapterAddDependencyFunction *addDependency;
	ShovelerComponentViewAdapterRemoveDependencyFunction *removeDependency;
	ShovelerComponentViewAdapterForEachReverseDependencyFunction *forEachReverseDependency;
	ShovelerComponentViewAdapterReportActivationFunction *reportActivation;
	void *userData;
} ShovelerComponentViewAdapter;

typedef struct ShovelerComponentStruct {
	ShovelerComponentViewAdapter *viewAdapter;
	long long int entityId;
	ShovelerComponentType *type;
	bool isAuthoritative;
	ShovelerComponentConfigurationValue *configurationValues;
	GQueue *dependencies;
	void *data;
} ShovelerComponent;

/**
 * Constructor for a configuration option.
 *
 * If the passed live update function is not NULL, this indicates that the configuration option can
 * be updated by calling that function instead of deactivating and reactivating the component.
 */
ShovelerComponentTypeConfigurationOption shovelerComponentTypeConfigurationOption(const char *name, ShovelerComponentConfigurationOptionType type, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate);
/**
 * Constructor for a dependency configuration option.
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
ShovelerComponentTypeConfigurationOption shovelerComponentTypeConfigurationOptionDependency(const char *name, const char *dependencyComponentTypeId, bool isArray, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate, ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction *updateDependency);

/**
 * Creates a new component type.
 *
 * The specified ID should be a statically defined string with external linkage that will be used
 * as unique identifier for the component type. It is a string so it can be easily printed as part
 * of log messages.
 *
 * If an activation function is specified and not NULL, it will be called whenever a component
 * instance of this type tries to activate. It must return a non-NULL data value if its activation
 * succeeds.
 *
 * If a deactivation function is specified, it will be called when a component instance of this
 * type deactivates. It is intended to free any memory or registrations performed by the activation
 * function and should leave the component instance in a state where it can be reactivated again at
 * any later point in time.
 *
 * If requiresAuthority is true, the component will only attempt to activate if it is currently
 * delegated authority.
 *
 * A component type can be created with any number of configuration options that will be
 * instantiated on each component instance of this type. The caller retains ownership of the passed
 * configuration options.
 */
ShovelerComponentType *shovelerComponentTypeCreate(const char *id, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate, bool requiresAuthority, int numConfigurationOptions, const ShovelerComponentTypeConfigurationOption *configurationOptions);
void shovelerComponentTypeFree(ShovelerComponentType *componentType);

ShovelerComponent *shovelerComponentCreate(ShovelerComponentViewAdapter *viewAdapter, long long int entityId, ShovelerComponentType *componentType);
/**
 * Updates a configuration option with the specified id on this component.
 *
 * If isCanonical is true, no authority check is performed and the update is applied without
 * invoking the authoritative update handler.
 */
bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, int id, const ShovelerComponentConfigurationValue *value, bool isCanonical);
bool shovelerComponentClearConfigurationOption(ShovelerComponent *component, int id, bool isCanonical);
const ShovelerComponentConfigurationValue *shovelerComponentGetConfigurationValue(ShovelerComponent *component, int id);
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
void shovelerComponentDelegate(ShovelerComponent *component);
bool shovelerComponentIsAuthoritative(ShovelerComponent *component);
void shovelerComponentUndelegate(ShovelerComponent *component);
void *shovelerComponentGetViewTarget(ShovelerComponent *component, const char *targetName);
ShovelerComponent *shovelerComponentGetDependency(ShovelerComponent *component, int id);
ShovelerComponent *shovelerComponentGetArrayDependency(ShovelerComponent *component, int id, int index);
void shovelerComponentFree(ShovelerComponent *component);

GString *shovelerComponentConfigurationValuePrint(const ShovelerComponentConfigurationValue *configurationValue);

/**
 * A ShovelerComponentTypeConfigurationOptionLiveUpdateFunction that does nothing and can be
 * passed to shovelerComponentTypeAddConfigurationOption.
 */
static inline void shovelerComponentLiveUpdateNoop(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	// deliberately do nothing
}

static inline bool shovelerComponentUpdateConfigurationOptionEntityId(ShovelerComponent *component, int id, long long int entityIdValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	value.isSet = true;
	value.entityIdValue = entityIdValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionEntityIdArray(ShovelerComponent *component, int id, const long long int *entityIds, size_t size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY;
	value.isSet = true;
	value.entityIdArrayValue.entityIds = (long long int *) entityIds; // won't be modified
	value.entityIdArrayValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionFloat(ShovelerComponent *component, int id, float floatValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT;
	value.isSet = true;
	value.floatValue = floatValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionBool(ShovelerComponent *component, int id, bool boolValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL;
	value.isSet = true;
	value.boolValue = boolValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionInt(ShovelerComponent *component, int id, int intValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT;
	value.isSet = true;
	value.intValue = intValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionString(ShovelerComponent *component, int id, const char *stringValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING;
	value.isSet = true;
	value.stringValue = (char *) stringValue; // won't be modified
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector2(ShovelerComponent *component, int id, ShovelerVector2 vector2Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2;
	value.isSet = true;
	value.vector2Value = vector2Value;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector3(ShovelerComponent *component, int id, ShovelerVector3 vector3Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3;
	value.isSet = true;
	value.vector3Value = vector3Value;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionVector4(ShovelerComponent *component, int id, ShovelerVector4 vector4Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4;
	value.isSet = true;
	value.vector4Value = vector4Value;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateConfigurationOptionBytes(ShovelerComponent *component, int id, const unsigned char *data, size_t size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES;
	value.isSet = true;
	value.bytesValue.data = (unsigned char *) data; // won't be modified
	value.bytesValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ false);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionEntityId(ShovelerComponent *component, int id, long long int entityIdValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	value.isSet = true;
	value.entityIdValue = entityIdValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(ShovelerComponent *component, int id, const long long int *entityIds, int size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY;
	value.isSet = true;
	value.entityIdArrayValue.entityIds = (long long int *) entityIds; // won't be modified
	value.entityIdArrayValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionFloat(ShovelerComponent *component, int id, float floatValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT;
	value.isSet = true;
	value.floatValue = floatValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionBool(ShovelerComponent *component, int id, bool boolValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL;
	value.isSet = true;
	value.boolValue = boolValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionInt(ShovelerComponent *component, int id, int intValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT;
	value.isSet = true;
	value.intValue = intValue;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionString(ShovelerComponent *component, int id, const char *stringValue)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING;
	value.isSet = true;
	value.stringValue = (char *) stringValue; // won't be modified
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionVector2(ShovelerComponent *component, int id, ShovelerVector2 vector2Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2;
	value.isSet = true;
	value.vector2Value = vector2Value;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionVector3(ShovelerComponent *component, int id, ShovelerVector3 vector3Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3;
	value.isSet = true;
	value.vector3Value = vector3Value;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionVector4(ShovelerComponent *component, int id, ShovelerVector4 vector4Value)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4;
	value.isSet = true;
	value.vector4Value = vector4Value;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentUpdateCanonicalConfigurationOptionBytes(ShovelerComponent *component, int id, const unsigned char *data, int size)
{
	ShovelerComponentConfigurationValue value;
	value.type = SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES;
	value.isSet = true;
	value.bytesValue.data = (unsigned char *) data; // won't be modified
	value.bytesValue.size = size;
	return shovelerComponentUpdateConfigurationOption(component, id, &value, /* isCanonical */ true);
}

static inline bool shovelerComponentHasConfigurationValue(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);

	return configurationValue->isSet;
}

static inline long long int shovelerComponentGetConfigurationValueEntityId(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID);

	return configurationValue->entityIdValue;
}

static inline void shovelerComponentGetConfigurationValueEntityIdArray(ShovelerComponent *component, int id, const long long int **outputEntityIds, size_t *outputSize)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY);

	*outputEntityIds = configurationValue->entityIdArrayValue.entityIds;
	*outputSize = configurationValue->entityIdArrayValue.size;
}

static inline float shovelerComponentGetConfigurationValueFloat(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT);

	return configurationValue->floatValue;
}

static inline bool shovelerComponentGetConfigurationValueBool(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL);

	return configurationValue->boolValue;
}

static inline int shovelerComponentGetConfigurationValueInt(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT);

	return configurationValue->intValue;
}

static inline const char *shovelerComponentGetConfigurationValueString(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING);

	return configurationValue->stringValue;
}

static inline ShovelerVector2 shovelerComponentGetConfigurationValueVector2(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2);

	return configurationValue->vector2Value;
}

static inline ShovelerVector3 shovelerComponentGetConfigurationValueVector3(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3);

	return configurationValue->vector3Value;
}

static inline ShovelerVector4 shovelerComponentGetConfigurationValueVector4(ShovelerComponent *component, int id)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4);

	return configurationValue->vector4Value;
}

static inline void shovelerComponentGetConfigurationValueBytes(ShovelerComponent *component, int id, const unsigned char **outputData, int *outputSize)
{
	const ShovelerComponentConfigurationValue *configurationValue = shovelerComponentGetConfigurationValue(component, id);
	assert(configurationValue->isSet);
	assert(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES);

	if(outputData != NULL) {
		*outputData = configurationValue->bytesValue.data;
	}

	if(outputSize != NULL) {
		*outputSize = configurationValue->bytesValue.size;
	}
}

#endif
