#ifndef SHOVELER_RESOURCES_H
#define SHOVELER_RESOURCES_H

#include <stddef.h> // size_t

#include <glib.h>

struct ShovelerResourceTypeLoaderStruct; // forward declaration
struct ShovelerResourcesStruct; // forward declaration

typedef struct {
	struct ShovelerResourcesStruct *resources;
	/** unique resource id owned by the object */
	char *id;
	/** static resource type id not owned by the object */
	const char *typeId;
	/** type specific resource data */
	void *data;
} ShovelerResource;

typedef void *(ShovelerResourcesTypeLoaderLoadFunction)(struct ShovelerResourceTypeLoaderStruct *typeLoader, const unsigned char *buffer, size_t bytes);
typedef void (ShovelerResourcesTypeLoaderFreeResourceFunction)(struct ShovelerResourceTypeLoaderStruct *typeLoader, void *resourceData);
typedef void (ShovelerResourcesTypeLoaderFreeFunction)(struct ShovelerResourceTypeLoaderStruct *typeLoader);

typedef struct ShovelerResourceTypeLoaderStruct {
	const char *typeId;
	void *defaultResource;
	void *data;
	ShovelerResourcesTypeLoaderLoadFunction *load;
	ShovelerResourcesTypeLoaderFreeResourceFunction *freeResourceData;
	ShovelerResourcesTypeLoaderFreeFunction *free;
} ShovelerResourcesTypeLoader;

typedef void (ShovelerResourcesRequestFunction)(struct ShovelerResourcesStruct *resources, const char *typeId, const char *resourceId, void *userData);

typedef struct ShovelerResourcesStruct {
	ShovelerResourcesRequestFunction *request;
	void *requestUserData;
	/** map from (const char *) resource type id to (ShovelerResourcesTypeLoader *) */
	GHashTable *typeLoaders;
	/** map from (char *) resource id to (ShovelerResource *) */
	GHashTable *resources;
} ShovelerResources;

ShovelerResources *shovelerResourcesCreate(ShovelerResourcesRequestFunction *request, void *userData);
bool shovelerResourcesRegisterTypeLoader(ShovelerResources *resources, ShovelerResourcesTypeLoader typeLoader);
ShovelerResource *shovelerResourcesGet(ShovelerResources *resources, const char *typeId, const char *resourceId);
void shovelerResourcesSet(ShovelerResources *resources, const char *typeId, const char *resourceId, const unsigned char *buffer, size_t bytes);
void shovelerResourcesFree(ShovelerResources *resources);

#endif
