#ifndef SHOVELER_RESOURCES_H
#define SHOVELER_RESOURCES_H

#include <glib.h>

struct ShovelerResourcesTypeLoaderStruct; // forward declaration
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

typedef void *(ShovelerResourcesTypeLoaderLoadFunction)(struct ShovelerResourcesTypeLoaderStruct *typeLoader, const unsigned char *buffer, int bufferSize);
typedef void (ShovelerResourcesTypeLoaderFreeResourceFunction)(struct ShovelerResourcesTypeLoaderStruct *typeLoader, void *resourceData);
typedef void (ShovelerResourcesTypeLoaderFreeFunction)(struct ShovelerResourcesTypeLoaderStruct *typeLoader);

typedef struct ShovelerResourcesTypeLoaderStruct {
	const char *typeId;
	/** the default resource gets freed automatically using freeResourceData on freeing of the typeloader before free is called */
	void *defaultResourceData;
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
bool shovelerResourcesSet(ShovelerResources *resources, const char *typeId, const char *resourceId, const unsigned char *buffer, int bufferSize);
void shovelerResourcesFree(ShovelerResources *resources);

#endif
