#include "shoveler/component/texture.h"

#include "shoveler/component/image.h"
#include "shoveler/component.h"
#include "shoveler/texture.h"

const char *const shovelerComponentTypeIdTexture = "texture";

static void *activateTextureComponent(ShovelerComponent *component);
static void deactivateTextureComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTextureType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[1];
	configurationOptions[SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE] = shovelerComponentTypeConfigurationOptionDependency("image", shovelerComponentTypeIdImage, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTexture, activateTextureComponent, /* update */ NULL, deactivateTextureComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerTexture *shovelerComponentGetTexture(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTexture);

	return component->data;
}

static void *activateTextureComponent(ShovelerComponent *component)
{
	ShovelerComponent *imageComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE);
	assert(imageComponent != NULL);
	ShovelerImage *image = shovelerComponentGetImage(imageComponent);
	assert(image != NULL);

	ShovelerTexture *texture = shovelerTextureCreate2d(image, false);
	shovelerTextureUpdate(texture);

	return texture;
}

static void deactivateTextureComponent(ShovelerComponent *component)
{
	ShovelerTexture *texture = (ShovelerTexture *) component->data;

	shovelerTextureFree(texture);
}
