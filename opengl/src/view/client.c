#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/client.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdClient)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateClientType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdClient);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CLIENT_OPTION_ID_POSITION, configuration->positionEntityId);
	if(configuration->modelEntityId != 0) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CLIENT_OPTION_ID_MODEL, configuration->modelEntityId);
	}

	shovelerComponentActivate(component);
	return component;
}

bool shovelerViewEntityGetClientConfiguration(ShovelerViewEntity *entity, ShovelerViewClientConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdClient);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_CLIENT_OPTION_ID_POSITION);
	if(shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_CLIENT_OPTION_ID_MODEL)) {
		outputConfiguration->modelEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_CLIENT_OPTION_ID_MODEL);
	}
	return true;
}

bool shovelerViewEntityUpdateClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdClient);
	if(component == NULL) {
		shovelerLogWarning("Trying to update client of entity %lld which does not have a client, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CLIENT_OPTION_ID_POSITION, configuration->positionEntityId);
	if(configuration->modelEntityId != 0) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_CLIENT_OPTION_ID_MODEL, configuration->modelEntityId);
	}
}

bool shovelerViewEntityRemoveClient(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdClient);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove client from entity %lld which does not have a client, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdClient);
}
