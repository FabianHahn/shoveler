#include <assert.h> // assert
#include <stdlib.h> // malloc free NULL
#include <string.h> // strdup

#include <glib.h>

#include "shoveler/view/resources.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewResourceConfiguration configuration;
	char *resourceId;
} ResourceComponentData;

static void assignConfiguration(ShovelerViewResourceConfiguration *destination, ShovelerViewResourceConfiguration *source);
static void clearConfiguration(ShovelerViewResourceConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddResource(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add resource to entity %lld which already has a resource, ignoring.", entity->entityId);
		return false;
	}

	GString *resourceId = g_string_new("");
	g_string_append_printf(resourceId, "%lld", entity->entityId);

	ResourceComponentData *componentData = malloc(sizeof(ResourceComponentData));
	componentData->configuration.typeId = NULL;
	componentData->configuration.buffer = NULL;
	componentData->configuration.bufferSize = 0;

	assignConfiguration(&componentData->configuration, &configuration);
	componentData->resourceId = resourceId->str;

	g_string_free(resourceId, false);

	component = shovelerViewEntityAddComponent(entity, shovelerViewResourceComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentActivate(component);
	return true;
}

void *shovelerViewEntityGetResource(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasResources(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		return NULL;
	}

	ResourceComponentData *componentData = component->data;
	ShovelerResources *resources = shovelerViewGetResources(component->entity->view);
	ShovelerResource *resource = shovelerResourcesGet(resources, componentData->configuration.typeId, componentData->resourceId);
	return resource->data;
}

const ShovelerViewResourceConfiguration *shovelerViewEntityGetResourceConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		return NULL;
	}

	ResourceComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateResource(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update resource for entity %lld which does not have a resource, ignoring.", entity->entityId);
		return false;
	}

	shovelerViewComponentDeactivate(component);

	ResourceComponentData *componentData = component->data;
	assignConfiguration(&componentData->configuration, &configuration);
	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
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

static void assignConfiguration(ShovelerViewResourceConfiguration *destination, ShovelerViewResourceConfiguration *source)
{
	clearConfiguration(destination);

	unsigned char *newBuffer = malloc(source->bufferSize * sizeof(unsigned char));
	memcpy(newBuffer, source->buffer, source->bufferSize);

	destination->typeId = strdup(source->typeId);
	destination->buffer = newBuffer;
	destination->bufferSize = source->bufferSize;
}

static void clearConfiguration(ShovelerViewResourceConfiguration *configuration)
{
	if(configuration->typeId != NULL) {
		free((void *) configuration->typeId);
		configuration->typeId = NULL;
	}

	if(configuration->buffer != NULL) {
		free((void *) configuration->buffer);
		configuration->buffer = NULL;
	}

	configuration->bufferSize = 0;
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	assert(shovelerViewHasResources(component->entity->view));
	ResourceComponentData *componentData = componentDataPointer;

	ShovelerResources *resources = shovelerViewGetResources(component->entity->view);
	return shovelerResourcesSet(resources, componentData->configuration.typeId, componentData->resourceId, componentData->configuration.buffer, componentData->configuration.bufferSize);
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	// nothing to do here, resources are currently persisted
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ResourceComponentData *componentData = componentDataPointer;

	clearConfiguration(&componentData->configuration);
	free(componentData->resourceId);
	free(componentData);
}
