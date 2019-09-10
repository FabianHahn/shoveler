#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/position.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddPosition(ShovelerViewEntity *entity, ShovelerVector3 coordinates)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewPositionComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewPositionComponentTypeName, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewPositionCoordinatesOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, shovelerComponentLiveUpdateNoop);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewPositionComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewPositionCoordinatesOptionKey, coordinates);

	shovelerComponentActivate(component);
	return component;
}

const ShovelerVector3 *shovelerViewEntityGetPositionCoordinates(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	const ShovelerComponentConfigurationValue *value = shovelerComponentGetConfigurationValue(component, shovelerViewPositionCoordinatesOptionKey);
	return &value->vector3Value;
}

bool shovelerViewEntityRemovePosition(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove position from entity %lld which does not have a position, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewPositionComponentTypeName);
}
