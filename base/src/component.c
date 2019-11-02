#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // strdup memcpy

#include "shoveler/component.h"
#include "shoveler/log.h"

typedef struct {
	long long int entityId;
	const char *componentTypeName;
} Dependency;

static ShovelerComponentConfigurationValue *createConfigurationValue(ShovelerComponentConfigurationOptionType type);
static void clearConfigurationValue(ShovelerComponentConfigurationValue *configurationValue);
static ShovelerComponentConfigurationValue *copyConfigurationValue(const ShovelerComponentConfigurationValue *reference);
static void assignConfigurationValue(ShovelerComponentConfigurationValue *target, const ShovelerComponentConfigurationValue *source);
static void freeConfigurationValue(void *configurationValuePointer);
static void updateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused);
static void activateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused);
static void deactivateReverseDependency(ShovelerComponent *sourceComponent, ShovelerComponent *targetComponent, void *unused);
static void addConfigurationOptionDependencies(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *configurationValue);
static void removeConfigurationOptionDependencies(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *configurationValue);
static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName);
static void removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName);
static bool checkDependenciesActive(ShovelerComponent *component);
static long long int toDependencyTargetEntityId(ShovelerComponent *component, long long int entityIdValue);
static void freeConfigurationOption(void *configurationOptionPointer);

ShovelerComponentType *shovelerComponentTypeCreate(const char *name, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate, bool requiresAuthority)
{
	ShovelerComponentType *componentType = malloc(sizeof(ShovelerComponentType));
	componentType->name = strdup(name);
	componentType->configurationOptions = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeConfigurationOption);
	componentType->activate = activate;
	componentType->deactivate = deactivate;
	componentType->requiresAuthority = requiresAuthority;

	return componentType;
}

bool shovelerComponentTypeAddConfigurationOption(ShovelerComponentType *componentType, const char *key, ShovelerComponentConfigurationOptionType type, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate)
{
	if(g_hash_table_lookup(componentType->configurationOptions, key) != NULL) {
		return false;
	}

	ShovelerComponentTypeConfigurationOption *configurationOption = malloc(sizeof(ShovelerComponentTypeConfigurationOption));
	configurationOption->key = strdup(key);
	configurationOption->type = type;
	configurationOption->isOptional = isOptional;
	configurationOption->liveUpdate = liveUpdate;
	configurationOption->updateDependency = NULL;
	configurationOption->dependencyComponentTypeName = NULL;

	g_hash_table_insert(componentType->configurationOptions, configurationOption->key, configurationOption);
	return true;
}

bool shovelerComponentTypeAddDependencyConfigurationOption(ShovelerComponentType *componentType, const char *key, const char *dependencyComponentTypeName, bool isArray, bool isOptional, ShovelerComponentTypeConfigurationOptionLiveUpdateFunction *liveUpdate, ShovelerComponentTypeConfigurationOptionUpdateDependencyFunction *updateDependency)
{
	if(g_hash_table_lookup(componentType->configurationOptions, key) != NULL) {
		return false;
	}

	ShovelerComponentTypeConfigurationOption *configurationOption = malloc(sizeof(ShovelerComponentTypeConfigurationOption));
	configurationOption->key = strdup(key);
	configurationOption->type = isArray ? SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY : SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID;
	configurationOption->isOptional = isOptional;
	configurationOption->liveUpdate = liveUpdate;
	configurationOption->updateDependency = updateDependency;
	configurationOption->dependencyComponentTypeName = strdup(dependencyComponentTypeName);

	g_hash_table_insert(componentType->configurationOptions, configurationOption->key, configurationOption);
	return true;
}

bool shovelerComponentTypeRemoveConfigurationOption(ShovelerComponentType *componentType, const char *key)
{
	return g_hash_table_remove(componentType->configurationOptions, key);
}

void shovelerComponentTypeFree(ShovelerComponentType *componentType)
{
	if(componentType == NULL) {
		return;
	}

	g_hash_table_destroy(componentType->configurationOptions);
	free(componentType->name);
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
	component->configurationValues = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeConfigurationValue);
	component->dependencies = g_queue_new();
	component->data = NULL;

	GHashTableIter iter;
	const char *key = NULL;
	ShovelerComponentTypeConfigurationOption *configurationOption = NULL;
	g_hash_table_iter_init(&iter, componentType->configurationOptions);
	while(g_hash_table_iter_next(&iter, (gpointer *) &key, (gpointer *) &configurationOption)) {
		if(configurationOption->isOptional) {
			continue;
		}

		ShovelerComponentConfigurationValue *configurationValue = createConfigurationValue(configurationOption->type);
		g_hash_table_insert(component->configurationValues, configurationOption->key, configurationValue);

		addConfigurationOptionDependencies(component, configurationOption, configurationValue);
	}

	return component;
}

bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, const char *key, const ShovelerComponentConfigurationValue *value, bool isCanonical)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = g_hash_table_lookup(component->type->configurationOptions, key);
	if(configurationOption == NULL) {
		return false;
	}

	if(value != NULL && configurationOption->type != value->type) {
		return false;
	}

	if(!isCanonical) {
		if(!component->isAuthoritative) {
			return false;
		}

		// TODO: invoke authoritative update listener
	}

	ShovelerComponentConfigurationValue *configurationValue = g_hash_table_lookup(component->configurationValues, key);

	bool wasActive = component->data != NULL;
	bool canLiveUpdate = configurationOption->liveUpdate != NULL;
	bool isDependencyUpdate = configurationOption->dependencyComponentTypeName != NULL;

	if(wasActive && !canLiveUpdate) {
		// cannot live update, so deactivate before updating
		shovelerComponentDeactivate(component);
	}

	if(isDependencyUpdate && configurationValue != NULL) {
		// remove the dependencies from the previous configuration value
		removeConfigurationOptionDependencies(component, configurationOption, configurationValue);
	}

	// update option to its new value
	if(configurationValue != NULL) {
		if(value != NULL) { // present option updated to new value
			assignConfigurationValue(configurationValue, value);
		} else { // present option being cleared
			if(configurationOption->isOptional) { // unset optional value
				bool removed = g_hash_table_remove(component->configurationValues, configurationOption->key);
				assert(removed);
				configurationValue = NULL;
			} else { // clear value
				clearConfigurationValue(configurationValue);
			}
		}
	} else {
		if(value != NULL) { // unset option being set
			configurationValue = copyConfigurationValue(value);
			g_hash_table_insert(component->configurationValues, configurationOption->key, configurationValue);
		} else { // unset option being unset
			// nothing to do
		}
	}

	if(isDependencyUpdate && configurationValue != NULL) {
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

const ShovelerComponentConfigurationValue *shovelerComponentGetConfigurationValue(ShovelerComponent *component, const char *key)
{
	return g_hash_table_lookup(component->configurationValues, key);
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
	shovelerLogTrace("Activated component '%s' of entity %lld.", component->type->name, component->entityId);

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
	shovelerLogTrace("Deactivated component '%s' of entity %lld.", component->type->name, component->entityId);
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

ShovelerComponent *shovelerComponentGetDependency(ShovelerComponent *component, const char *optionKey)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = g_hash_table_lookup(component->type->configurationOptions, optionKey);
	if(configurationOption == NULL) {
		return NULL;
	}

	if(configurationOption->dependencyComponentTypeName == NULL) {
		return NULL;
	}

	if(configurationOption->type != SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID) {
		return NULL;
	}

	ShovelerComponentConfigurationValue *configurationValue = g_hash_table_lookup(component->configurationValues, optionKey);
	if(configurationValue == NULL) {
		return NULL;
	}

	return component->viewAdapter->getComponent(component, configurationValue->entityIdValue, configurationOption->dependencyComponentTypeName, component->viewAdapter->userData);
}

ShovelerComponent *shovelerComponentGetArrayDependency(ShovelerComponent *component, const char *optionKey, int index)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = g_hash_table_lookup(component->type->configurationOptions, optionKey);
	if(configurationOption == NULL) {
		return NULL;
	}

	if(configurationOption->dependencyComponentTypeName == NULL) {
		return NULL;
	}

	if(configurationOption->type != SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY) {
		return NULL;
	}

	ShovelerComponentConfigurationValue *configurationValue = g_hash_table_lookup(component->configurationValues, optionKey);
	if(configurationValue == NULL) {
		return NULL;
	}

	if(index >= configurationValue->entityIdArrayValue.size) {
		return NULL;
	}

	return component->viewAdapter->getComponent(component, configurationValue->entityIdArrayValue.entityIds[index], configurationOption->dependencyComponentTypeName, component->viewAdapter->userData);
}

void shovelerComponentFree(ShovelerComponent *component)
{
	if(component == NULL) {
		return;
	}

	shovelerComponentDeactivate(component);

	GHashTableIter iter;
	const char *key = NULL;
	ShovelerComponentTypeConfigurationOption *configurationOption = NULL;
	g_hash_table_iter_init(&iter, component->type->configurationOptions);
	while(g_hash_table_iter_next(&iter, (gpointer *) &key, (gpointer *) &configurationOption)) {
		if(configurationOption->dependencyComponentTypeName != NULL) {
			ShovelerComponentConfigurationValue *configurationValue = g_hash_table_lookup(component->configurationValues, key);
			removeConfigurationOptionDependencies(component, configurationOption, configurationValue);
		}
	}

	assert(component->dependencies->length == 0);
	g_queue_free(component->dependencies);
	g_hash_table_destroy(component->configurationValues);
	free(component);
}

static ShovelerComponentConfigurationValue *createConfigurationValue(ShovelerComponentConfigurationOptionType type)
{
	ShovelerComponentConfigurationValue *copy = malloc(sizeof(ShovelerComponentConfigurationValue));
	memset(copy, 0, sizeof(ShovelerComponentConfigurationValue));
	copy->type = type;
	return copy;
}

static ShovelerComponentConfigurationValue *copyConfigurationValue(const ShovelerComponentConfigurationValue *reference)
{
	if(reference == NULL) {
		return NULL;
	}

	ShovelerComponentConfigurationValue *copy = createConfigurationValue(reference->type);
	assignConfigurationValue(copy, reference);
	return copy;
}

static void clearConfigurationValue(ShovelerComponentConfigurationValue *configurationValue)
{
	if(configurationValue == NULL) {
		return;
	}

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

	GHashTableIter configurationOptionIter;
	g_hash_table_iter_init(&configurationOptionIter, sourceComponent->type->configurationOptions);
	const char *key;
	ShovelerComponentTypeConfigurationOption *configurationOption;
	while(g_hash_table_iter_next(&configurationOptionIter, (gpointer *) &key, (gpointer *) &configurationOption)) {
		// check if this option is a dependency pointing to the target
		if(configurationOption->dependencyComponentTypeName == NULL || strcmp(configurationOption->dependencyComponentTypeName, targetComponent->type->name) != 0) {
			continue;
		}

		ShovelerComponentConfigurationValue *configurationValue = g_hash_table_lookup(sourceComponent->configurationValues, key);
		if(configurationValue == NULL) {
			// value is optional and isn't set
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
	if(configurationValue == NULL) {
		// this configuration option is optional
		assert(configurationOption->isOptional);
		return;
	}

	if(configurationOption->dependencyComponentTypeName == NULL) {
		// this configuration option isn't a dependency
		return;
	}

	assert(configurationOption->type == configurationValue->type);

	if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID) {
		addDependency(
			component,
			/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdValue),
			/* targetComponentTypeName */ configurationOption->dependencyComponentTypeName);
	} else if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY) {
		for(int i = 0; i < configurationValue->entityIdArrayValue.size; i++) {
			addDependency(
				component,
				/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdArrayValue.entityIds[i]),
				/* targetComponentTypeName */ configurationOption->dependencyComponentTypeName);
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
	if(configurationValue == NULL) {
		// this configuration option is optional
		assert(configurationOption->isOptional);
		return;
	}

	if(configurationOption->dependencyComponentTypeName == NULL) {
		// this configuration option isn't a dependency
		return;
	}

	assert(configurationOption->type == configurationValue->type);

	if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID) {
		removeDependency(
			component,
			/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdValue),
			/* targetComponentTypeName */ configurationOption->dependencyComponentTypeName);
	} else if(configurationOption->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID_ARRAY) {
		for(int i = 0; i < configurationValue->entityIdArrayValue.size; i++) {
			removeDependency(
				component,
				/* targetEntityId */ toDependencyTargetEntityId(component, configurationValue->entityIdArrayValue.entityIds[i]),
				/* targetComponentTypeName */ configurationOption->dependencyComponentTypeName);
		}
	}
}

static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName)
{
	Dependency *dependency = malloc(sizeof(Dependency));
	dependency->entityId = targetEntityId;
	dependency->componentTypeName = targetComponentTypeName;
	g_queue_push_tail(component->dependencies, dependency);

	component->viewAdapter->addDependency(component, targetEntityId, targetComponentTypeName, component->viewAdapter->userData);
}

static void removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName)
{
	for(GList *iter = component->dependencies->head; iter != NULL; iter = iter->next) {
		Dependency *dependency = iter->data;
		if(dependency->entityId == targetEntityId && strcmp(dependency->componentTypeName, targetComponentTypeName) == 0) {
			g_queue_unlink(component->dependencies, iter);
			g_list_free(iter);
			free(dependency);
			break;
		}
	}

	bool dependencyRemoved = component->viewAdapter->removeDependency(component, targetEntityId, targetComponentTypeName, component->viewAdapter->userData);
	assert(dependencyRemoved);
}

static bool checkDependenciesActive(ShovelerComponent *component)
{
	for(GList *iter = component->dependencies->head; iter != NULL; iter = iter->next) {
		Dependency *dependency = iter->data;
		ShovelerComponent *targetComponent = component->viewAdapter->getComponent(component, dependency->entityId, dependency->componentTypeName, component->viewAdapter->userData);
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

static void freeConfigurationOption(void *configurationOptionPointer)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = configurationOptionPointer;

	free(configurationOption->dependencyComponentTypeName);
	free(configurationOption->key);
	free(configurationOption);
}
