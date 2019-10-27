#include <assert.h> // assert
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, strcmp, strdup

#include "shoveler/log.h"
#include "shoveler/resources.h"

static void freeTypeLoader(void *typeLoaderPointer);
static void freeResource(void *resourcePointer);
static void freeResourceData(ShovelerResourcesTypeLoader *typeLoader, ShovelerResource *resource);

ShovelerResources *shovelerResourcesCreate(ShovelerResourcesRequestFunction *request, void *userData)
{
	ShovelerResources *resources = malloc(sizeof(ShovelerResources));
	resources->request = request;
	resources->requestUserData = userData;
	resources->typeLoaders = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeTypeLoader);
	resources->resources = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeResource);

	return resources;
}

bool shovelerResourcesRegisterTypeLoader(ShovelerResources *resources, ShovelerResourcesTypeLoader typeLoader)
{
	if(g_hash_table_lookup(resources->typeLoaders, typeLoader.typeId) != NULL) {
		shovelerLogError("Failed to register resources type loader for '%s' because a type loader for this type already exists", typeLoader.typeId);
		return false;
	}

	ShovelerResourcesTypeLoader *typeLoaderCopy = malloc(sizeof(ShovelerResourcesTypeLoader));
	memcpy(typeLoaderCopy, &typeLoader, sizeof(ShovelerResourcesTypeLoader));
	return g_hash_table_insert(resources->typeLoaders, (void *) typeLoader.typeId, typeLoaderCopy);
}

ShovelerResource *shovelerResourcesGet(ShovelerResources *resources, const char *typeId, const char *resourceId)
{
	ShovelerResourcesTypeLoader *typeLoader = (ShovelerResourcesTypeLoader *) g_hash_table_lookup(resources->typeLoaders, typeId);
	if(typeLoader == NULL) {
		shovelerLogError("Failed to request resource '%s' of unknown type '%s'.", resourceId, typeId);
		return NULL;
	}

	ShovelerResource *resource = (ShovelerResource *) g_hash_table_lookup(resources->resources, resourceId);
	if(resource == NULL) {
		shovelerLogTrace("Requested unloaded resource '%s' of type '%s', loading...", resourceId, typeId);

		resource = malloc(sizeof(ShovelerResource));
		resource->resources = resources;
		resource->id = strdup(resourceId);
		resource->typeId = typeId;
		resource->data = typeLoader->defaultResourceData;

		g_hash_table_insert(resources->resources, resource->id, resource);

		if (resources->request != NULL) {
			resources->request(resources, typeId, resourceId, resources->requestUserData);
		}
	}

	if(strcmp(resource->typeId, typeId) != 0) {
		shovelerLogError("Requested resource '%s' of type '%s' but it is already loaded as type '%d'.", resourceId, typeId, resource->typeId);
		return NULL;
	}

	return resource;
}

bool shovelerResourcesSet(ShovelerResources *resources, const char *typeId, const char *resourceId, const unsigned char *buffer, int bufferSize)
{
	ShovelerResourcesTypeLoader *typeLoader = (ShovelerResourcesTypeLoader *) g_hash_table_lookup(resources->typeLoaders, typeId);
	if(typeLoader == NULL) {
		shovelerLogError("Failed to load resource '%s' of unknown type '%s' (%d bytes).", resourceId, typeId, bufferSize);
		return false;
	}

	ShovelerResource *resource = (ShovelerResource *) g_hash_table_lookup(resources->resources, resourceId);
	if(resource == NULL) {
		shovelerLogTrace("Loading unrequested resource '%s' of type '%s' (%d bytes).", resourceId, typeId, bufferSize);

		resource = malloc(sizeof(ShovelerResource));
		resource->resources = resources;
		resource->id = strdup(resourceId);
		resource->typeId = typeLoader->typeId;
		resource->data = typeLoader->defaultResourceData;

		g_hash_table_insert(resources->resources, resource->id, resource);
	} else {
		shovelerLogTrace("Loading previously requested resource '%s' of type '%s' (%d bytes).", resourceId, typeId, bufferSize);
	}

	freeResourceData(typeLoader, resource);

	resource->data = typeLoader->load(typeLoader, buffer, bufferSize);
	if(resource->data == NULL) {
		shovelerLogWarning("Failed to load resource '%s' of type '%s' (%d bytes), reverting to default resource data.", resourceId, typeId, bufferSize);
		resource->data = typeLoader->defaultResourceData;
		return false;
	}

	shovelerLogTrace("Loaded resource '%s' of type '%s'.", resourceId, typeId);
	return true;
}

void shovelerResourcesFree(ShovelerResources *resources)
{
	if(resources == NULL) {
		return;
	}

	g_hash_table_destroy(resources->resources);
	g_hash_table_destroy(resources->typeLoaders);
	free(resources);
}

static void freeTypeLoader(void *typeLoaderPointer)
{
	ShovelerResourcesTypeLoader *typeLoader = (ShovelerResourcesTypeLoader *) typeLoaderPointer;

	if(typeLoader->freeResourceData != NULL) {
		typeLoader->freeResourceData(typeLoader, typeLoader->defaultResourceData);
	}

	if(typeLoader->free != NULL) {
		typeLoader->free(typeLoader);
	}

	free(typeLoader);
}

static void freeResource(void *resourcePointer)
{
	ShovelerResource *resource = (ShovelerResource *) resourcePointer;

	ShovelerResourcesTypeLoader *typeLoader = (ShovelerResourcesTypeLoader *) g_hash_table_lookup(resource->resources->typeLoaders, resource->typeId);
	assert(typeLoader != NULL);

	freeResourceData(typeLoader, resource);
	free(resource->id);
	free(resource);
}

static void freeResourceData(ShovelerResourcesTypeLoader *typeLoader, ShovelerResource *resource)
{
	if(typeLoader->freeResourceData != NULL && resource->data != typeLoader->defaultResourceData) {
		typeLoader->freeResourceData(typeLoader, resource->data);
	}
}
