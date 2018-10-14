#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/resources.h"
#include "shoveler/view/texture.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTextureConfiguration configuration;
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
} TextureComponentData;

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddTexture(ShovelerViewEntity *entity, ShovelerViewTextureConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add texture to entity %lld which already has a texture, ignoring.", entity->entityId);
		return false;
	}

	TextureComponentData *componentData = malloc(sizeof(TextureComponentData));
	componentData->configuration = configuration;
	componentData->texture = NULL;
	componentData->sampler = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewTextureComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, configuration.imageResourceEntityId, shovelerViewResourceComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerTexture *shovelerViewEntityGetTexture(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentName);
	if(component == NULL) {
		return NULL;
	}

	TextureComponentData *componentData = component->data;
	return componentData->texture;
}

ShovelerSampler *shovelerViewEntityGetTextureSampler(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentName);
	if(component == NULL) {
		return NULL;
	}

	TextureComponentData *componentData = component->data;
	return componentData->sampler;
}

bool shovelerViewEntityUpdateTexture(ShovelerViewEntity *entity, ShovelerViewTextureConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update texture of entity %lld which does not have a texture, ignoring.", entity->entityId);
		return false;
	}

	TextureComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	if(!shovelerViewComponentRemoveDependency(component, configuration.imageResourceEntityId, shovelerViewResourceComponentName)) {
		return false;
	}

	componentData->configuration = configuration;
	shovelerViewComponentAddDependency(component, configuration.imageResourceEntityId, shovelerViewResourceComponentName);
	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTexture(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTextureComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove texture from entity %lld which does not have a texture, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTextureComponentName);
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TextureComponentData *componentData = componentDataPointer;

	ShovelerViewEntity *imageResourceEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.imageResourceEntityId);
	assert(imageResourceEntity != NULL);
	ShovelerImage *image = shovelerViewEntityGetResource(imageResourceEntity);
	assert(image != NULL);

	componentData->texture = shovelerTextureCreate2d(image, false);
	shovelerTextureUpdate(componentData->texture);

	componentData->sampler = shovelerSamplerCreate(componentData->configuration.interpolate, componentData->configuration.clamp);
	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TextureComponentData *componentData = componentDataPointer;

	shovelerSamplerFree(componentData->sampler);
	componentData->sampler = NULL;

	shovelerTextureFree(componentData->texture);
	componentData->texture = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TextureComponentData *componentData = componentDataPointer;

	if(componentData->texture != NULL) {
		shovelerTextureFree(componentData->texture);
	}

	if(componentData->sampler != NULL) {
		shovelerSamplerFree(componentData->sampler);
	}

	free(componentData);
}
