#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/client.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNameClient)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateClientType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNameClient);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentClientOptionKeyPosition, configuration->positionEntityId);
	if(configuration->modelEntityId != 0) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentClientOptionKeyModel, configuration->modelEntityId);
	}

	shovelerComponentActivate(component);
	return component;
}

bool shovelerViewEntityGetClientConfiguration(ShovelerViewEntity *entity, ShovelerViewClientConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameClient);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentClientOptionKeyPosition);
	if(shovelerComponentHasConfigurationValue(component, shovelerComponentClientOptionKeyModel)) {
		outputConfiguration->modelEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentClientOptionKeyModel);
	}
	return true;
}

bool shovelerViewEntityUpdateClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameClient);
	if(component == NULL) {
		shovelerLogWarning("Trying to update client of entity %lld which does not have a client, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentClientOptionKeyPosition, configuration->positionEntityId);
	if(configuration->modelEntityId != 0) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentClientOptionKeyModel, configuration->modelEntityId);
	}
}

bool shovelerViewEntityRemoveClient(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameClient);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove client from entity %lld which does not have a client, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNameClient);
}
