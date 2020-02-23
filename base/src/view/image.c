#include "shoveler/view/image.h"

#include "shoveler/component.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddImage(ShovelerViewEntity *entity, const ShovelerViewImageConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdImage)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateImageType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdImage);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT, configuration->format);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE, configuration->resourceEntityId);

	shovelerComponentActivate(component);
	return component;
}


ShovelerImage *shovelerViewEntityGetImage(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdImage);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetImageConfiguration(ShovelerViewEntity *entity, ShovelerViewImageConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdImage);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->format = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT);
	outputConfiguration->resourceEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE);
	return true;
}

bool shovelerViewEntityUpdateImageConfiguration(ShovelerViewEntity *entity, const ShovelerViewImageConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdImage);
	if(component == NULL) {
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT, configuration->format);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE, configuration->resourceEntityId);
	return true;
}

bool shovelerViewEntityRemoveImage(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdImage);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove image from entity %lld which does not have a image, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdImage);
}
