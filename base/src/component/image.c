#include "shoveler/component/image.h"

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/image.h"
#include "shoveler/image/png.h"
#include "shoveler/image/ppm.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdImage = "image";

static void *activateImageComponent(ShovelerComponent *component);
static void deactivateImageComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateImageType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[2];
	configurationOptions[SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT] = shovelerComponentTypeConfigurationOption("format", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE] = shovelerComponentTypeConfigurationOptionDependency("resource", shovelerComponentTypeIdResource, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdImage, activateImageComponent, /* update */ NULL, deactivateImageComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerImage *shovelerComponentGetImage(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdImage);

	return component->data;
}

static void *activateImageComponent(ShovelerComponent *component)
{
	ShovelerComponent *resourceComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE);
	assert(resourceComponent != NULL);

	const unsigned char *bufferData;
	int bufferSize;
	shovelerComponentGetResource(resourceComponent, &bufferData, &bufferSize);

	ShovelerComponentImageFormat format = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT);
	switch(format) {
		case SHOVELER_COMPONENT_IMAGE_FORMAT_PNG:
			return shovelerImagePngReadBuffer(bufferData, bufferSize);
		case SHOVELER_COMPONENT_IMAGE_FORMAT_PPM:
			return shovelerImagePpmReadBuffer(bufferData, bufferSize);
		default:
			shovelerLogWarning("Failed to activate entity %lld component image: Unknown format value %d.", component->entityId, format);
			return NULL;
	}
}

static void deactivateImageComponent(ShovelerComponent *component)
{
	ShovelerImage *image = component->data;

	shovelerImageFree(image);
}
