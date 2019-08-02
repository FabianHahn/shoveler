#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // strdup

#include "shoveler/component.h"

static void freeConfigurationOption(void *configurationOptionPointer);

ShovelerComponentType *shovelerComponentTypeCreate(const char *name, ShovelerComponentTypeActivationFunction *activate, ShovelerComponentTypeDeactivationFunction *deactivate, void *callbackUserData)
{
	ShovelerComponentType *componentType = malloc(sizeof(ShovelerComponentType));
	componentType->name = strdup(name);
	componentType->callbackUserData = callbackUserData;
	componentType->configurationOptions = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeConfigurationOption);
	componentType->activate = activate;
	componentType->deactivate = deactivate;

	return componentType;
}

bool shovelerComponentTypeAddConfigurationOption(ShovelerComponentType *componentType, const char *key, ShovelerComponentTypeConfigurationOptionType type, ShovelerComponentConfigurationValue defaultValue, ShovelerComponentTypeConfigurationOptionUpdateFunction *update)
{
	if(g_hash_table_lookup(componentType->configurationOptions, key) != NULL) {
		return false;
	}

	ShovelerComponentTypeConfigurationOption *configurationOption = malloc(sizeof(ShovelerComponentTypeConfigurationOption));
	configurationOption->key = strdup(key);
	configurationOption->type = type;
	configurationOption->defaultValue = defaultValue;
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

ShovelerComponent *shovelerComponentCreate(ShovelerComponentType *type)
{
	ShovelerComponent *component = malloc(sizeof(ShovelerComponent));
	component->type = type;
	component->configurationValues = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
	component->data = NULL;

	GHashTableIter iter;
	const char *key = NULL;
	ShovelerComponentTypeConfigurationOption *configurationOption = NULL;
	g_hash_table_iter_init(&iter, type->configurationOptions);
	while(g_hash_table_iter_next(&iter, (gpointer *) key, (gpointer *) configurationOption)) {
		ShovelerComponentConfigurationValue *configurationValue = malloc(sizeof(ShovelerComponentConfigurationValue));
		*configurationValue = configurationOption->defaultValue;
		g_hash_table_insert(component->configurationValues, configurationOption->key, configurationOption);
	}

	return component;
}

bool shovelerComponentUpdateConfigurationOption(ShovelerComponent *component, const char *key, ShovelerComponentConfigurationValue value)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = g_hash_table_lookup(component->type->configurationOptions, key);
	if(configurationOption == NULL) {
		return false;
	}

	ShovelerComponentConfigurationValue *configurationValue = g_hash_table_lookup(component->configurationValues, key);
	assert(configurationValue != NULL);

	if(configurationOption->update == NULL) {
		shovelerComponentDeactivate(component);
	}

	ShovelerComponentConfigurationValue previousValue = *configurationValue;
	*configurationValue = value;

	if(configurationOption->update == NULL) {
		shovelerComponentActivate(component);
	} else {
		configurationOption->update(component, configurationOption, previousValue, component->type->callbackUserData);
	}

	return true;
}

bool shovelerComponentActivate(ShovelerComponent *component)
{
	if(component->data != NULL) {
		return false;
	}

	component->data = component->type->activate(component, component->type->callbackUserData);
	return component->data != NULL;
}

bool shovelerComponentDeactivate(ShovelerComponent *component)
{
	if(component->data == NULL) {
		return false;
	}

	component->type->deactivate(component, component->type->callbackUserData);
	component->data = NULL;

	return true;
}

void shovelerComponentFree(ShovelerComponent *component)
{
	g_hash_table_destroy(component->configurationValues);
	free(component);
}

static void freeConfigurationOption(void *configurationOptionPointer)
{
	ShovelerComponentTypeConfigurationOption *configurationOption = configurationOptionPointer;

	free(configurationOption->key);
	free(configurationOption);
}
