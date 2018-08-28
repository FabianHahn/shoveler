#include <assert.h> // assert
#include <stdlib.h> // malloc free NULL
#include <string.h> // strdup

#include <glib.h>

#include "shoveler/view/resources.h"
#include "shoveler/log.h"

static void freeResourceComponentData(ShovelerViewComponent *component);

bool shovelerViewAddEntityResource(ShovelerView *view, long long int entityId, const char *typeId, const unsigned char *buffer, size_t bytes)
{
	assert(shovelerViewHasResources(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to add resource to non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add resource to entity %lld which already has a resource, ignoring.", entityId);
		return false;
	}

	GString *resourceId = g_string_new("");
	g_string_append_printf(resourceId, "%lld", entityId);

	ShovelerViewResource *resource = malloc(sizeof(ShovelerViewResource));
	resource->typeId = strdup(typeId);
	resource->resourceId = resourceId->str;

	g_string_free(resourceId, false);

	ShovelerResources *resources = shovelerViewGetResources(view);
	shovelerResourcesSet(resources, typeId, resource->resourceId, buffer, bytes);

	if (!shovelerViewEntityAddComponent(entity, shovelerViewResourceComponentName, resource, freeResourceComponentData)) {
		return false;
	}
	return true;
}

bool shovelerViewUpdateEntityResource(ShovelerView *view, long long int entityId, const unsigned char *buffer, size_t bytes)
{
	assert(shovelerViewHasResources(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update resource for non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update resource for entity %lld which does not have a resource, ignoring.", entityId);
		return false;
	}

	ShovelerViewResource *resource = (ShovelerViewResource *) component->data;
	ShovelerResources *resources = shovelerViewGetResources(view);
	shovelerResourcesSet(resources, resource->typeId, resource->resourceId, buffer, bytes);

	return shovelerViewEntityUpdateComponent(entity, shovelerViewResourceComponentName);
}

bool shovelerViewRemoveEntityResource(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to remove resource from non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewResourceComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove resource from entity %lld which does not have a resource, ignoring.", entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewResourceComponentName);
}

static void freeResourceComponentData(ShovelerViewComponent *component)
{
	ShovelerViewResource *resource = component->data;
	free(resource->typeId);
	free(resource->resourceId);
	free(resource);
}
