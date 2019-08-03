#include <assert.h> // assert
#include <stdlib.h> // malloc free NULL
#include <string.h> // strdup

#include <glib.h>

#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

static unsigned char defaultBufferData = 0;

static void *activateResourceComponent(ShovelerComponent *component, void *viewComponentPointer);
static void deactivateResourceComponent(ShovelerComponent *component, void *viewComponentPointer);

bool shovelerViewEntityAddResource(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewResourceComponentName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewResourceComponentName, activateResourceComponent, deactivateResourceComponent);
		shovelerComponentTypeAddConfigurationOptionString(componentType, shovelerViewResourceTypeIdOptionKey, "", NULL);
		shovelerComponentTypeAddConfigurationOptionBytes(componentType, shovelerViewResourceBufferOptionKey, &defaultBufferData, 1, NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerViewComponent *viewComponent = shovelerViewEntityAddTypedComponent(entity, shovelerViewResourceComponentName);
	shovelerComponentUpdateConfigurationOptionString(viewComponent->data, shovelerViewResourceTypeIdOptionKey, configuration.typeId);
	shovelerComponentUpdateConfigurationOptionBytes(viewComponent->data, shovelerViewResourceBufferOptionKey, configuration.buffer, configuration.bufferSize);

	shovelerViewComponentActivate(viewComponent);
	return true;
}

void *shovelerViewEntityGetResource(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasResources(entity->view));

	ShovelerComponent *component = shovelerViewEntityGetTypedComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetResourceConfiguration(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetTypedComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->typeId = shovelerComponentGetConfigurationValueString(component, shovelerViewResourceTypeIdOptionKey);
	shovelerComponentGetConfigurationValueBytes(component, shovelerViewResourceBufferOptionKey, &outputConfiguration->buffer, &outputConfiguration->bufferSize);
	return true;
}

bool shovelerViewEntityRemoveResource(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove resource from entity %lld which does not have a resource, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewResourceComponentName);
}

static void *activateResourceComponent(ShovelerComponent *component, void *viewComponentPointer)
{
	ShovelerViewComponent *viewComponent = viewComponentPointer;
	assert(shovelerViewHasResources(viewComponent->entity->view));

	ShovelerResources *resources = shovelerViewGetResources(viewComponent->entity->view);

	GString *resourceId = g_string_new("");
	g_string_append_printf(resourceId, "%lld", viewComponent->entity->entityId);

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

static void deactivateResourceComponent(ShovelerComponent *component, void *viewComponentPointer)
{
	// nothing to do here, resources are currently persisted
}

