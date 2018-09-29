#include <assert.h> // assert
#include <stdlib.h> // malloc free NULL
#include <string.h> // strdup

#include <glib.h>

#include "shoveler/view/resources.h"
#include "shoveler/log.h"

static void freeResourceComponentData(ShovelerViewComponent *component, void *resourcePointer);

bool shovelerViewEntityAddResource(ShovelerViewEntity *entity, const char *typeId, const unsigned char *buffer, size_t bytes)
{
	assert(shovelerViewHasResources(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add resource to entity %lld which already has a resource, ignoring.", entity->entityId);
		return false;
	}

	GString *resourceId = g_string_new("");
	g_string_append_printf(resourceId, "%lld", entity->entityId);

	ShovelerViewResource *resource = malloc(sizeof(ShovelerViewResource));
	resource->typeId = strdup(typeId);
	resource->resourceId = resourceId->str;

	g_string_free(resourceId, false);

	ShovelerResources *resources = shovelerViewGetResources(entity->view);
	shovelerResourcesSet(resources, typeId, resource->resourceId, buffer, bytes);

	component = shovelerViewEntityAddComponent(entity, shovelerViewResourceComponentName, resource, NULL, NULL, freeResourceComponentData);
	assert(component != NULL);

	shovelerViewComponentActivate(component);
	return true;
}

bool shovelerViewEntityUpdateResource(ShovelerViewEntity *entity, const unsigned char *buffer, size_t bytes)
{
	assert(shovelerViewHasResources(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update resource for entity %lld which does not have a resource, ignoring.", entity->entityId);
		return false;
	}

	ShovelerViewResource *resource = (ShovelerViewResource *) component->data;
	ShovelerResources *resources = shovelerViewGetResources(entity->view);
	shovelerResourcesSet(resources, resource->typeId, resource->resourceId, buffer, bytes);

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

static void freeResourceComponentData(ShovelerViewComponent *component, void *resourcePointer)
{
	ShovelerViewResource *resource = component->data;
	free(resource->typeId);
	free(resource->resourceId);
	free(resource);
}
