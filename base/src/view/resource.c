#include <assert.h> // assert
#include <stdlib.h> // malloc free NULL
#include <string.h> // strdup

#include <glib.h>

#include "shoveler/component/resource.h"
#include "shoveler/view/resource.h"
#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/resources.h"

const char *const shovelerComponentTypeIdResource = "resource";

ShovelerComponent *shovelerViewEntityAddResource(ShovelerViewEntity *entity, const ShovelerViewResourceConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdResource)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateResourceType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdResource);
	shovelerComponentUpdateCanonicalConfigurationOptionString(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_TYPE_ID, configuration->typeId);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, configuration->buffer, configuration->bufferSize);

	shovelerComponentActivate(component);
	return component;
}

void *shovelerViewEntityGetResource(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdResource);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetResourceConfiguration(ShovelerViewEntity *entity, ShovelerViewResourceConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdResource);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->typeId = shovelerComponentGetConfigurationValueString(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_TYPE_ID);
	shovelerComponentGetConfigurationValueBytes(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, &outputConfiguration->buffer, &outputConfiguration->bufferSize);
	return true;
}

bool shovelerViewEntityUpdateResourceConfiguration(ShovelerViewEntity *entity, const ShovelerViewResourceConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdResource);
	if(component == NULL) {
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionString(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_TYPE_ID, configuration->typeId);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, configuration->buffer, configuration->bufferSize);
	return true;
}

bool shovelerViewEntityRemoveResource(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdResource);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove resource from entity %lld which does not have a resource, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdResource);
}
