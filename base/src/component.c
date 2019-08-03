#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // strdup

#include "shoveler/component.h"

static ShovelerComponentConfigurationValue *copyConfigurationValue(const ShovelerComponentConfigurationValue *reference);
static void assignConfigurationValue(ShovelerComponentConfigurationValue *target, const ShovelerComponentConfigurationValue *source);
static void freeConfigurationValue(void *configurationValuePointer);
static void freeConfigurationOption(void *configurationOptionPointer);

ShovelerComponentType *shovelerComponentTypeCreate(const char *name, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate)
{
	ShovelerComponentType *componentType = malloc(sizeof(ShovelerComponentType));
	componentType->name = strdup(name);
	componentType->configurationOptions = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeConfigurationOption);
	componentType->activate = activate;
	componentType->deactivate = deactivate;

	return componentType;
}

bool shovelerComponentTypeAddConfigurationOption(ShovelerComponentType *componentType, const char *key, ShovelerComponentConfigurationOptionType type, const ShovelerComponentConfigurationValue *defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	if(g_hash_table_lookup(componentType->configurationOptions, key) != NULL) {
		return false;
	}

	ShovelerComponentTypeConfigurationOption *configurationOption = malloc(sizeof(ShovelerComponentTypeConfigurationOption));
	configurationOption->key = strdup(key);
	configurationOption->type = type;
	configurationOption->defaultValue = copyConfigurationValue(defaultValue);
	configurationOption->update = update;

	g_hash_table_insert(componentType->configurationOptions, configurationOption->key, configurationOption);
	return true;
}

bool shovelerComponentTypeRemoveConfigurationOption(ShovelerComponentType *componentType, const char *key)
{
	return g_hash_table_remove(componentType->configurationOptions, key);
}

void shovelerComponentTypeFree(ShovelerComponentType *componentType)
{
	g_hash_table_destroy(componentType->configurationOptions);
	free(componentType->name);
	free(componentType);
}

ShovelerComponent *shovelerComponentCreate(ShovelerComponentType *type, void *callbackUserData)
{
	ShovelerComponent *component = malloc(sizeof(ShovelerComponent));
	component->type = type;
	component->configurationValues = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeConfigurationValue);
	component->data = NULL;
	component->callbackUserData = callbackUserData;

	GHashTableIter iter;
	const char *key = NULL;
	ShovelerComponentTypeConfigurationOption *configurationOption = NULL;
	g_hash_table_iter_init(&iter, type->configurationOptions);
	while(g_hash_table_iter_next(&iter, (gpointer *) &key, (gpointer *) &configurationOption)) {
		g_hash_table_insert(component->configurationValues, configurationOption->key, copyConfigurationValue(configurationOption->defaultValue));
	}

	return component;
}

bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, const char *key, const ShovelerComponentConfigurationValue *value)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = g_hash_table_lookup(component->type->configurationOptions, key);
	if(configurationOption == NULL) {
		return false;
	}

	if(configurationOption->type != value->type) {
		return false;
	}

	ShovelerComponentConfigurationValue *configurationValue = g_hash_table_lookup(component->configurationValues, key);
	assert(configurationValue != NULL);

	if(component->data != NULL) {
		if(configurationOption->update == NULL) {
			shovelerComponentDeactivate(component);
		}

		assignConfigurationValue(configurationValue, value);

		if(configurationOption->update == NULL) {
			shovelerComponentActivate(component);
		} else {
			configurationOption->update(component, configurationOption, configurationValue, component->callbackUserData);
		}
	} else {
		assignConfigurationValue(configurationValue, value);
	}

	return true;
}

const ShovelerComponentConfigurationValue *shovelerComponentGetConfigurationValue(ShovelerComponent *component, const char *key)
{
	return g_hash_table_lookup(component->configurationValues, key);
}

bool shovelerComponentActivate(ShovelerComponent *component)
{
	if(component->data != NULL) {
		return false;
	}

	component->data = component->type->activate(component, component->callbackUserData);
	return component->data != NULL;
}

bool shovelerComponentIsActive(ShovelerComponent *component)
{
	return component->data != NULL;
}

bool shovelerComponentDeactivate(ShovelerComponent *component)
{
	if(component->data == NULL) {
		return false;
	}

	component->type->deactivate(component, component->callbackUserData);
	component->data = NULL;

	return true;
}

void shovelerComponentFree(ShovelerComponent *component)
{
	shovelerComponentDeactivate(component);
	g_hash_table_destroy(component->configurationValues);
	free(component);
}

static ShovelerComponentConfigurationValue *copyConfigurationValue(const ShovelerComponentConfigurationValue *reference)
{
	ShovelerComponentConfigurationValue *copy = malloc(sizeof(ShovelerComponentConfigurationValue));
	copy->type = reference->type;

	switch(reference->type) {
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID:
			copy->entityIdValue = reference->entityIdValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT:
			copy->floatValue = reference->floatValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL:
			copy->boolValue = reference->boolValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT:
			copy->intValue = reference->intValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT:
			copy->uintValue = reference->uintValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING:
			copy->stringValue = strdup(reference->stringValue);
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2:
			copy->vector2Value = reference->vector2Value;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3:
			copy->vector3Value = reference->vector3Value;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4:
			copy->vector4Value = reference->vector4Value;
			break;
	}

	return copy;
}

static void assignConfigurationValue(ShovelerComponentConfigurationValue *target, const ShovelerComponentConfigurationValue *source)
{
	assert(target->type == source->type);

	switch(target->type) {
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_ENTITY_ID:
			target->entityIdValue = source->entityIdValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT:
			target->floatValue = source->floatValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL:
			target->boolValue = source->boolValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT:
			target->intValue = source->intValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT:
			target->uintValue = source->uintValue;
			break;
		case SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING:
			free(target->stringValue);
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
	}
}

static void freeConfigurationValue(void *configurationValuePointer)
{
	ShovelerComponentConfigurationValue *configurationValue = configurationValuePointer;

	if(configurationValue->type == SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING) {
		free(configurationValue->stringValue);
	}

	free(configurationValue);
}

static void freeConfigurationOption(void *configurationOptionPointer)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = configurationOptionPointer;

	freeConfigurationValue(configurationOption->defaultValue);
	free(configurationOption->key);
	free(configurationOption);
}
