#include "shoveler/component/texture.h"

#include <string.h> // strcmp

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/texture.h"

static void *activateTextureComponent(ShovelerComponent *component);
static void deactivateTextureComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTextureType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTextureComponentTypeName, activateTextureComponent, deactivateTextureComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTextureImageResourceOptionKey, shovelerComponentTypeNameResource, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}

ShovelerTexture *shovelerComponentGetTexture(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewTextureComponentTypeName) == 0);

	return component->data;
}

static void *activateTextureComponent(ShovelerComponent *component)
{
	long long int imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTextureImageResourceOptionKey);
	ShovelerComponent *imageResourceComponent = shovelerComponentGetDependency(component, shovelerViewTextureImageResourceOptionKey);
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
