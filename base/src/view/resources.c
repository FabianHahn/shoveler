#include <assert.h> // assert
#include <stdlib.h> // malloc free NULL
#include <string.h> // strdup

#include <glib.h>

#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/resources.h"

static void *activateResourceComponent(ShovelerComponent *component);
static void deactivateResourceComponent(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddResource(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewResourceComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewResourceComponentTypeName, activateResourceComponent, deactivateResourceComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewResourceTypeIdOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewResourceBufferOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewResourceComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionString(component, shovelerViewResourceTypeIdOptionKey, configuration.typeId);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewResourceBufferOptionKey, configuration.buffer, configuration.bufferSize);

	shovelerComponentActivate(component);
	return component;
}

void *shovelerViewEntityGetResource(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetResourceConfiguration(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->typeId = shovelerComponentGetConfigurationValueString(component, shovelerViewResourceTypeIdOptionKey);
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewResourceBufferOptionKey, &outputConfiguration->buffer, &outputConfiguration->bufferSize);
	return true;
}

bool shovelerViewEntityUpdateResourceConfiguration(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentTypeName);
	if(component == NULL) {
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionString(component, shovelerViewResourceTypeIdOptionKey, configuration.typeId);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, shovelerViewResourceBufferOptionKey, configuration.buffer, configuration.bufferSize);
	return true;
}

bool shovelerViewEntityRemoveResource(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove resource from entity %lld which does not have a resource, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewResourceComponentTypeName);
}

static void *activateResourceComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasResources(component->entity->view));

	ShovelerResources *resources = shovelerViewGetResources(component->entity->view);

	GString *resourceId = g_string_new("");
	g_string_append_printf(resourceId, "%lld", component->entity->entityId);

	const char *typeId = shovelerComponentGetConfigurationValueString(component, shovelerViewResourceTypeIdOptionKey);
	const unsigned char *bufferData;
	size_t bufferSize;
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewResourceBufferOptionKey, &bufferData, &bufferSize);
	if(!shovelerResourcesSet(resources, typeId, resourceId->str, bufferData, bufferSize)) {
		g_string_free(resourceId, true);
		return NULL;
	}

	ShovelerResource *resource = shovelerResourcesGet(resources, typeId, resourceId->str);
	g_string_free(resourceId, true);

	return resource->data;
}

static void deactivateResourceComponent(ShovelerComponent *component)
{
	// nothing to do here, resources are currently persisted
}

