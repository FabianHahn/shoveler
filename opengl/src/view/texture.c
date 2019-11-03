#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/resources.h"
#include "shoveler/view/texture.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

bool shovelerViewEntityAddTexture(ShovelerViewEntity *entity, const ShovelerViewTextureConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdTexture)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTextureType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdTexture);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE_RESOURCE, configuration->imageResourceEntityId);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTexture *shovelerViewEntityGetTexture(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTexture);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetTextureConfiguration(ShovelerViewEntity *entity, ShovelerViewTextureConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTexture);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->imageResourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE_RESOURCE);
	return true;
}

bool shovelerViewEntityUpdateTexture(ShovelerViewEntity *entity, const ShovelerViewTextureConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTexture);
	if(component == NULL) {
		shovelerLogWarning("Trying to update texture of entity %lld which does not have a texture, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE_RESOURCE, configuration->imageResourceEntityId);
	return true;
}

bool shovelerViewEntityRemoveTexture(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTexture);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove texture from entity %lld which does not have a texture, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdTexture);
}
