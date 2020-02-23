#include "shoveler/view/resource.h"

#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/resources.h"

ShovelerComponent *shovelerViewEntityAddResource(ShovelerViewEntity *entity, const ShovelerViewResourceConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdResource)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateResourceType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdResource);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(component, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, configuration->buffer, configuration->bufferSize);

	shovelerComponentActivate(component);
	return component;
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
