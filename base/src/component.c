#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // strdup memcpy

#include "shoveler/component.h"
#include "shoveler/log.h"

typedef struct {
	long long int entityId;
	const char *componentTypeId;
} Dependency;

static ShovelerComponentConfigurationValue *createConfigurationValue(ShovelerComponentConfigurationOptionType type);
static void initConfigurationValue(ShovelerComponentConfigurationValue *configurationValue, ShovelerComponentConfigurationOptionType type);
static void clearConfigurationValue(ShovelerComponentConfigurationValue *configurationValue);
static ShovelerComponentConfigurationValue *copyConfigurationValue(const ShovelerComponentConfigurationValue *reference);
static void assignConfigurationValue(ShovelerComponentConfigurationValue *target, const ShovelerComponentConfigurationValue *source);
static void freeConfigurationValue(void *configurationValuePointer);
static void updateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused);
static void activateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused);
static void deactivateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused);
static void addConfigurationOptionDependencies(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *configurationValue);
static void removeConfigurationOptionDependencies(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *configurationValue);
static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId);
static void removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId);
static bool checkDependenciesActive(ShovelerComponent *component);
static long long int toDependencyTargetEntityId(ShovelerComponent *component, long long int entityIdValue);

ShovelerComponentTypeConfigurationOption shovelerComponentTypeConfigurationOption(const char *name, ShovelerComponentConfigurationOptionType type, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate)
{
	ShovelerComponentTypeConfigurationOption configurationOption;
	configurationOption.name = name;
	configurationOption.type = type;
	configurationOption.isOptional = isOptional;
	configurationOption.liveUpdate = liveUpdate;
	configurationOption.updateDependency = NULL;
	configurationOption.dependencyComponentTypeId = NULL;

	return configurationOption;
}

ShovelerComponentTypeConfigurationOption shovelerComponentTypeConfigurationOptionDependency(const char *name, const char *dependencyComponentTypeId, bool isArray, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate, ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction *updateDependency)
{
	ShovelerComponentTypeConfigurationOption configurationOption;
	configurationOption.name = name;
	configurationOption.type = isArray ? SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY : SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	configurationOption.isOptional = isOptional;
	configurationOption.liveUpdate = liveUpdate;
	configurationOption.updateDependency = updateDependency;
	configurationOption.dependencyComponentTypeId = dependencyComponentTypeId;

	return configurationOption;
}

ShovelerComponentType *shovelerComponentTypeCreate(const char *id, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate, bool requiresAuthority, int numConfigurationOptions, const ShovelerComponentTypeConfigurationOption *configurationOptions)
{
	assert(numConfigurationOptions >= 0);

	ShovelerComponentType *componentType = malloc(sizeof(ShovelerComponentType));
	componentType->id = id;
	componentType->numConfigurationOptions = numConfigurationOptions;
	componentType->configurationOptions = NULL;
	componentType->activate = activate;
	componentType->deactivate = deactivate;
	componentType->requiresAuthority = requiresAuthority;

	if(numConfigurationOptions > 0) {
		 componentType->configurationOptions = malloc((size_t) numConfigurationOptions * sizeof(ShovelerComponentTypeConfigurationOption));
		 for(int id = 0; id < numConfigurationOptions; id++) {
			 componentType->configurationOptions[id] = configurationOptions[id];
		 }
	}

	return componentType;
}

void shovelerComponentTypeFree(ShovelerComponentType *componentType)
{
	if(componentType == NULL) {
		return;
	}

	free(componentType->configurationOptions);
	free(componentType);
}

ShovelerComponent *shovelerComponentCreate(ShovelerComponentViewAdapter *viewAdapter, long long int entityId, ShovelerComponentType *componentType)
{
	ShovelerComponent *component = malloc(sizeof(ShovelerComponent));
	component->viewAdapter = viewAdapter;
	component->entityId = entityId;
	component->type = componentType;
	component->type = componentType;
	component->isAuthoritative = false;
	component->configurationValues = NULL;
	component->dependencies = g_queue_new();
	component->data = NULL;

	if(component->type->numConfigurationOptions > 0) {
		component->configurationValues = malloc((size_t) component->type->numConfigurationOptions * sizeof(ShovelerComponentConfigurationValue));

		for(int id = 0; id < component->type->numConfigurationOptions; id++) {
			const ShovelerComponentTypeConfigurationOption *configurationOption = &component->type->configurationOptions[id];
			ShovelerComponentConfigurationValue *configurationValue = &component->configurationValues[id];

			initConfigurationValue(configurationValue, configurationOption->type);

			if(!configurationOption->isOptional) {
				configurationValue->isSet = true; // initialize with default value
			}

			addConfigurationOptionDependencies(component, configurationOption, configurationValue);
		}
	}

	return component;
}

bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, int id, const ShovelerComponentConfigurationValue *value, bool isCanonical)
{
	assert(id >= 0);
	assert(id < component->type->numConfigurationOptions);

	const ShovelerComponentTypeConfigurationOption *configurationOption = &component->type->configurationOptions[id];
	ShovelerComponentConfigurationValue *configurationValue = &component->configurationValues[id];

	if(value != NULL && configurationOption->type != value->type) {
		return false;
	}

	if(!isCanonical) {
		if(!component->isAuthoritative) {
			return false;
		}

		// TODO: invoke authoritative update listener
	}

	bool wasActive = component->data != NULL;
	bool canLiveUpdate = configurationOption->liveUpdate != NULL;
	bool isDependencyUpdate = configurationOption->dependencyComponentTypeId != NULL;

	if(wasActive && !canLiveUpdate) {
		// cannot live update, so deactivate before updating
		shovelerComponentDeactivate(component);
	}

	if(isDependencyUpdate) {
		// remove the dependencies from the previous configuration value
		removeConfigurationOptionDependencies(component, configurationOption, configurationValue);
	}

	// update option to its new value
	assignConfigurationValue(configurationValue, value);

	if(isDependencyUpdate) {
		// Add the new dependencies, which might deactivate the component if the dependency isn't
		// satisfied.
		addConfigurationOptionDependencies(component, configurationOption, configurationValue);

		// update wasActive because component might have been deactivated
		wasActive = component->data != NULL;
	}

	if(wasActive) {
		if(canLiveUpdate) {
			// live update value
			configurationOption->liveUpdate(component, configurationOption, configurationValue);

			// update reverse dependencies
			component->viewAdapter->forEachReverseDependency(component, updateReverseDependency, /* callbackUserData */ NULL, component->viewAdapter->userData);
		} else {
			// cannot live update, so try reactivating again
			shovelerComponentActivate(component);
		}
	}

	return true;
}

bool shovelerComponentClearConfigurationOption(ShovelerComponent *component, int id, bool isCanonical)
{
	assert(id >= 0);
	assert(id < component->type->numConfigurationOptions);

	const ShovelerComponentTypeConfigurationOption *configurationOption = &component->type->configurationOptions[id];

	ShovelerComponentConfigurationValue configurationValue;
	initConfigurationValue(&configurationValue, configurationOption->type);

	return shovelerComponentUpdateConfigurationOption(component, id, &configurationValue, isCanonical);
}

const ShovelerComponentConfigurationValue *shovelerComponentGetConfigurationValue(ShovelerComponent *component, int id)
{
	assert(id >= 0);
	assert(id < component->type->numConfigurationOptions);

	return &component->configurationValues[id];
}

bool shovelerComponentActivate(ShovelerComponent *component)
{
	if(shovelerComponentIsActive(component)) {
		return true;
	}

	if(component->type->requiresAuthority && !component->isAuthoritative) {
		return false;
	}

	if(!checkDependenciesActive(component)) {
		return false;
	}

	if(component->type->activate != NULL) {
		component->data = component->type->activate(component);
		if(component->data == NULL) {
			return false;
		}
	} else {
		component->data = component;
	}

	component->viewAdapter->reportActivation(component, /* diff */ 1, component->viewAdapter->userData);
	shovelerLogTrace("Activated component '%s' of entity %lld.", component->type->id, component->entityId);

	component->viewAdapter->forEachReverseDependency(component, activateReverseDependency, /* callbackUserData */ NULL, component->viewAdapter->userData);

	return true;
}

bool shovelerComponentIsActive(ShovelerComponent *component)
{
	return component->data != NULL;
}

void shovelerComponentDeactivate(ShovelerComponent *component)
{
	if(component->data == NULL) {
		return;
	}

	component->viewAdapter->forEachReverseDependency(component, deactivateReverseDependency, /* callbackUserData */ NULL, component->viewAdapter->userData);

	if(component->type->deactivate != NULL) {
		component->type->deactivate(component);
	}
	component->data = NULL;

	component->viewAdapter->reportActivation(component, /* delta */ -1, component->viewAdapter->userData);
	shovelerLogTrace("Deactivated component '%s' of entity %lld.", component->type->id, component->entityId);
}

void shovelerComponentDelegate(ShovelerComponent *component)
{
	component->isAuthoritative = true;
}

bool shovelerComponentIsAuthoritative(ShovelerComponent *component)
{
	return component->isAuthoritative;
}

void shovelerComponentUndelegate(ShovelerComponent *component)
{
	if(shovelerComponentIsActive(component) && component->type->requiresAuthority) {
		shovelerComponentDeactivate(component);
	}

	component->isAuthoritative = false;
}

void *shovelerComponentGetViewTarget(ShovelerComponent *component, const char *targetName)
{
	return component->viewAdapter->getTarget(component, targetName, component->viewAdapter->userData);
}

ShovelerComponent *shovelerComponentGetDependency(ShovelerComponent *component, int id)
{
	assert(id >= 0);
	assert(id < component->type->numConfigurationOptions);

	const ShovelerComponentTypeConfigurationOption *configurationOption = &component->type->configurationOptions[id];
	ShovelerComponentConfigurationValue *configurationValue = &component->configurationValues[id];

	if(configurationOption->dependencyComponentTypeId == NULL) {
		return NULL;
	}

	if(configurationOption->type != SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID) {
		return NULL;
	}

	return component->viewAdapter->getComponent(component, configurationValue->entityIdValue, configurationOption->dependencyComponentTypeId, component->viewAdapter->userData);
}

ShovelerComponent *shovelerComponentGetArrayDependency(ShovelerComponent *component, int id, int index)
{
	assert(id >= 0);
	assert(id < component->type->numConfigurationOptions);

	const ShovelerComponentTypeConfigurationOption *configurationOption = &component->type->configurationOptions[id];
	ShovelerComponentConfigurationValue *configurationValue = &component->configurationValues[id];

	if(configurationOption->dependencyComponentTypeId == NULL) {
		return NULL;
	}

	if(configurationOption->type != SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY) {
		return NULL;
	}

	if(index >= configurationValue->entityIdArrayValue.size) {
		return NULL;
	}

	return component->viewAdapter->getComponent(component, configurationValue->entityIdArrayValue.entityIds[index], configurationOption->dependencyComponentTypeId, component->viewAdapter->userData);
}

void shovelerComponentFree(ShovelerComponent *component)
{
	if(component == NULL) {
		return;
	}

	shovelerComponentDeactivate(component);

	for(int id = 0; id < component->type->numConfigurationOptions; id++) {
		const ShovelerComponentTypeConfigurationOption *configurationOption = &component->type->configurationOptions[id];
		ShovelerComponentConfigurationValue *configurationValue = &component->configurationValues[id];

		removeConfigurationOptionDependencies(component, configurationOption, configurationValue);
	}
	assert(component->dependencies->length == 0);
	g_queue_free(component->dependencies);

	for(int id = 0; id < component->type->numConfigurationOptions; id++) {
		ShovelerComponentConfigurationValue *configurationValue = &component->configurationValues[id];

		clearConfigurationValue(configurationValue);
	}
	free(component->configurationValues);

	free(component);
}

static ShovelerComponentConfigurationValue *createConfigurationValue(ShovelerComponentConfigurationOptionType type)
{
	ShovelerComponentConfigurationValue *configurationValue = malloc(sizeof(ShovelerComponentConfigurationValue));
	initConfigurationValue(configurationValue, type);
	return configurationValue;
}

static void initConfigurationValue(ShovelerComponentConfigurationValue *configurationValue, ShovelerComponentConfigurationOptionType type)
{
	memset(configurationValue, 0, sizeof(ShovelerComponentConfigurationValue));
	configurationValue->type = type;
	configurationValue->isSet = false;
}

static ShovelerComponentConfigurationValue *copyConfigurationValue(const ShovelerComponentConfigurationValue *reference)
{
	ShovelerComponentConfigurationValue *copy = createConfigurationValue(reference->type);
	assignConfigurationValue(copy, reference);
	return copy;
}

static void clearConfigurationValue(ShovelerComponentConfigurationValue *configurationValue)
{
	configurationValue->isSet = false;

	switch(configurationValue->type) {
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID:
			configurationValue->entityIdValue = 0;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY:
			free(configurationValue->entityIdArrayValue.entityIds);
			configurationValue->entityIdArrayValue.entityIds = NULL;
			configurationValue->entityIdArrayValue.size = 0;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT:
			configurationValue->floatValue = 0.0f;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL:
			configurationValue->boolValue = false;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT:
			configurationValue->intValue = 0;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING:
			free(configurationValue->stringValue);
			configurationValue->stringValue = NULL;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2:
			configurationValue->vector2Value = shovelerVector2(0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3:
			configurationValue->vector3Value = shovelerVector3(0.0f, 0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4:
			configurationValue->vector4Value = shovelerVector4(0.0f, 0.0f, 0.0f, 0.0f);
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES:
			free(configurationValue->bytesValue.data);
			configurationValue->bytesValue.data = NULL;
			configurationValue->bytesValue.size = 0;
			break;
	}
}

static void assignConfigurationValue(ShovelerComponentConfigurationValue *target, const ShovelerComponentConfigurationValue *source)
{
	assert(source != NULL);
	assert(target != NULL);
	assert(target->type == source->type);

	clearConfigurationValue(target);

	target->isSet = source->isSet;
	if(!target->isSet) {
		return;
	}

	switch(target->type) {
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID:
			target->entityIdValue = source->entityIdValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY: {
			if(source->entityIdArrayValue.size > 0) {
				size_t numElements = (size_t) source->entityIdArrayValue.size;

				target->entityIdArrayValue.entityIds = malloc(numElements * sizeof(long long int));
				target->entityIdArrayValue.size = source->entityIdArrayValue.size;
				memcpy(target->entityIdArrayValue.entityIds, source->entityIdArrayValue.entityIds, numElements * sizeof(long long int));
			}
		} break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT:
			target->floatValue = source->floatValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL:
			target->boolValue = source->boolValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT:
			target->intValue = source->intValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING:
			target->stringValue = strdup(source->stringValue);
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2:
			target->vector2Value = source->vector2Value;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3:
			target->vector3Value = source->vector3Value;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4:
			target->vector4Value = source->vector4Value;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES: {
			if(source->entityIdArrayValue.size > 0) {
				size_t numBytes = (size_t) source->bytesValue.size;

				target->bytesValue.data = malloc(numBytes * sizeof(unsigned char));
				target->bytesValue.size = source->bytesValue.size;
				memcpy(target->bytesValue.data, source->bytesValue.data, numBytes * sizeof(unsigned char));
			}
		} break;
	}
}

static void freeConfigurationValue(void *configurationValuePointer)
{
	ShovelerComponentConfigurationValue *configurationValue = configurationValuePointer;

	clearConfigurationValue(configurationValue);
	free(configurationValue);
}

static void updateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused)
{
	if(sourceComponent->data == NULL) {
		// no need to update the reverse dependency if it isn't active
		return;
	}

	for(int id = 0; id < sourceComponent->type->numConfigurationOptions; id++) {
		const ShovelerComponentTypeConfigurationOption *configurationOption = &sourceComponent->type->configurationOptions[id];
		ShovelerComponentConfigurationValue *configurationValue = &sourceComponent->configurationValues[id];

		// check if this option is a dependency pointing to the target
		if(configurationOption->dependencyComponentTypeId != targetComponent->type->id) {
			continue;
		}

		if(!configurationValue->isSet) {
			assert(configurationOption->isOptional);
			continue;
		}

		if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID) {
			if(configurationValue->entityIdValue != targetComponent->entityId) {
				continue;
			}
		} else {
			assert(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY);

			bool requiresUpdate = false;
			for(int i = 0; i < configurationValue->entityIdArrayValue.size; i++) {
				if(configurationValue->entityIdArrayValue.entityIds[i] == targetComponent->entityId) {
					requiresUpdate = true;
					break;
				}
			}

			if(!requiresUpdate) {
				continue;
			}
		}

		if(configurationOption->updateDependency != NULL) {
			configurationOption->updateDependency(sourceComponent, configurationOption, targetComponent);
		}

		// recursively update reverse dependencies
		sourceComponent->viewAdapter->forEachReverseDependency(sourceComponent, updateReverseDependency, /* callbackUserData */ NULL, sourceComponent->viewAdapter->userData);
	}
}


static void activateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused)
{
	shovelerComponentActivate(sourceComponent);
}

static void deactivateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused)
{
	shovelerComponentDeactivate(sourceComponent);
}

static void addConfigurationOptionDependencies(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *configurationValue)
{
	if(!configurationValue->isSet) {
		// this configuration option is optional
		assert(configurationOption->isOptional);
		return;
	}

	if(configurationOption->dependencyComponentTypeId == NULL) {
		// this configuration option isn't a dependency
		return;
	}

	assert(configurationOption->type == configurationValue->type);

	if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID) {
		addDependency(
			component,
			/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdValue),
			/* targetComponentTypeId */ configurationOption->dependencyComponentTypeId);
	} else if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY) {
		for(int i = 0; i < configurationValue->entityIdArrayValue.size; i++) {
			addDependency(
				component,
				/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdArrayValue.entityIds[i]),
				/* targetComponentTypeId */ configurationOption->dependencyComponentTypeId);
		}
	}

	// if we are activate and one of the added dependencies isn't, we need to deactivate
	if(component->data != NULL) {
		if(!checkDependenciesActive(component)) {
			shovelerComponentDeactivate(component);
		}
	}
}

static void removeConfigurationOptionDependencies(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *configurationValue)
{
	if(!configurationValue->isSet) {
		// this configuration option is optional
		assert(configurationOption->isOptional);
		return;
	}

	if(configurationOption->dependencyComponentTypeId == NULL) {
		// this configuration option isn't a dependency
		return;
	}

	assert(configurationOption->type == configurationValue->type);

	if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID) {
		removeDependency(
			component,
			/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdValue),
			/* targetComponentTypeId */ configurationOption->dependencyComponentTypeId);
	} else if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY) {
		for(int i = 0; i < configurationValue->entityIdArrayValue.size; i++) {
			removeDependency(
				component,
				/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdArrayValue.entityIds[i]),
				/* targetComponentTypeId */ configurationOption->dependencyComponentTypeId);
		}
	}
}

static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId)
{
	Dependency *dependency = malloc(sizeof(Dependency));
	dependency->entityId = targetEntityId;
	dependency->componentTypeId = targetComponentTypeId;
	g_queue_push_tail(component->dependencies, dependency);

	component->viewAdapter->addDependency(component, targetEntityId, targetComponentTypeId, component->viewAdapter->userData);
}

static void removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId)
{
	for(GList *iter = component->dependencies->head; iter != NULL; iter = iter->next) {
		Dependency *dependency = iter->data;
		if(dependency->entityId == targetEntityId && dependency->componentTypeId == targetComponentTypeId) {
			g_queue_unlink(component->dependencies, iter);
			g_list_free(iter);
			free(dependency);
			break;
		}
	}

	bool dependencyRemoved = component->viewAdapter->removeDependency(component, targetEntityId, targetComponentTypeId, component->viewAdapter->userData);
	assert(dependencyRemoved);
}

static bool checkDependenciesActive(ShovelerComponent *component)
{
	for(GList *iter = component->dependencies->head; iter != NULL; iter = iter->next) {
		Dependency *dependency = iter->data;
		ShovelerComponent *targetComponent = component->viewAdapter->getComponent(component, dependency->entityId, dependency->componentTypeId, component->viewAdapter->userData);
		if(targetComponent == NULL) {
			return false;
		}

		if(!shovelerComponentIsActive(targetComponent)) {
			return false;
		}
	}

	return true;
}

static long long int toDependencyTargetEntityId(ShovelerComponent *component, long long int entityIdValue)
{
	if(entityIdValue != 0) {
		return entityIdValue;
	} else {
		return component->entityId;
	}
}
