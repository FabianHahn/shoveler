#include "shoveler/component/resource.h"

#include "shoveler/component.h"
#include "shoveler/resources.h"

const char *const shovelerComponentTypeIdResource = "resource";

static void *activateResourceComponent(ShovelerComponent *component);
static void deactivateResourceComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateResourceType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[1];
	configurationOptions[SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER] = shovelerComponentTypeConfigurationOption("type_id", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BYTES, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdResource, /* activate */ NULL, /* update */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

void shovelerComponentGetResource(ShovelerComponent *component, const unsigned char **outputData, int *outputSize)
{
	assert(component->type->id == shovelerComponentTypeIdResource);

	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, outputData, outputSize);
}
