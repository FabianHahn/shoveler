#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/resources.h"
#include "shoveler/view/texture.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

/*
 * typedef struct {
	long long int imageResourceEntityId;
} ShovelerViewTextureConfiguration;

static const char *shovelerViewTextureComponentTypeName = "texture";
static const char *shovelerViewTextureImageResourceOptionKey = "image_resource";
*/

static void *activateTextureComponent(ShovelerComponent *component);
static void deactivateTextureComponent(ShovelerComponent *component);

bool shovelerViewEntityAddTexture(ShovelerViewEntity *entity, const ShovelerViewTextureConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTextureComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTextureComponentTypeName, activateTextureComponent, deactivateTextureComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTextureImageResourceOptionKey, shovelerViewResourceComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTextureComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTextureImageResourceOptionKey, configuration->imageResourceEntityId);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTexture *shovelerViewEntityGetTexture(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTextureConfiguration(ShovelerViewEntity *entity, ShovelerViewTextureConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTextureImageResourceOptionKey);
	return true;
}

bool shovelerViewEntityUpdateTexture(ShovelerViewEntity *entity, const ShovelerViewTextureConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update texture of entity %lld which does not have a texture, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTextureImageResourceOptionKey, configuration->imageResourceEntityId);
	return true;
}

bool shovelerViewEntityRemoveTexture(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove texture from entity %lld which does not have a texture, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTextureComponentTypeName);
}

static void *activateTextureComponent(ShovelerComponent *component)
{
	long long int imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTextureImageResourceOptionKey);
	ShovelerViewEntity *imageResourceEntity = shovelerViewGetEntity(component->entity->view, imageResourceEntityId);
	assert(imageResourceEntity != NULL);
	ShovelerImage *image = shovelerViewEntityGetResource(imageResourceEntity);
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
