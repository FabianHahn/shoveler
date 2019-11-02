#include "shoveler/component/resource.h"

#include <string.h>

#include "shoveler/component/resources.h"
#include "shoveler/component.h"
#include "shoveler/resources.h"

static void *activateResourceComponent(ShovelerComponent *component);
static void deactivateResourceComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateResourceType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[2];
	configurationOptions[SHOVELER_COMPONENT_RESOURCE_OPTION_TYPE_ID] = shovelerComponentTypeConfigurationOption("type_id", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_RESOURCE_OPTION_BUFFER] = shovelerComponentTypeConfigurationOption("type_id", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdResource, activateResourceComponent, deactivateResourceComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

void *shovelerComponentGetResource(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdResource) == 0);

	return component->data;
}

static void *activateResourceComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewResources(component));

	ShovelerResources *resources = shovelerComponentGetViewResources(component);

	GString *resourceId = g_string_new("");
	g_string_append_printf(resourceId, "%lld", component->entityId);

	const char *typeId = shovelerComponentGetConfigurationValueString(component, SHOVELER_COMPONENT_RESOURCE_OPTION_TYPE_ID);
	const unsigned char *bufferData;
	int bufferSize;
	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_RESOURCE_OPTION_BUFFER, &bufferData, &bufferSize);
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
