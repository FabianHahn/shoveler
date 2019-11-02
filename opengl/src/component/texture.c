#include "shoveler/component/texture.h"

#include <string.h> // strcmp

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/texture.h"

const char *const shovelerComponentTypeIdTexture = "texture";

static void *activateTextureComponent(ShovelerComponent *component);
static void deactivateTextureComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTextureType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdTexture, activateTextureComponent, deactivateTextureComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentTextureOptionKeyImageResource, shovelerComponentTypeIdResource, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}

ShovelerTexture *shovelerComponentGetTexture(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdTexture) == 0);

	return component->data;
}

static void *activateTextureComponent(ShovelerComponent *component)
{
	long long int imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentTextureOptionKeyImageResource);
	ShovelerComponent *imageResourceComponent = shovelerComponentGetDependency(component, shovelerComponentTextureOptionKeyImageResource);
	assert(imageResourceComponent != NULL);
	ShovelerImage *image = shovelerComponentGetResource(imageResourceComponent);
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
