#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/component/position.h"
#include "shoveler/view/position.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddPosition(ShovelerViewEntity *entity, ShovelerVector3 coordinates)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNamePosition)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreatePositionType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNamePosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerComponentPositionOptionKeyCoordinates, coordinates);

	shovelerComponentActivate(component);
	return component;
}

const ShovelerVector3 *shovelerViewEntityGetPositionCoordinates(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNamePosition);
	if(component == NULL) {
		return NULL;
	}

	const ShovelerComponentConfigurationValue *value = shovelerComponentGetConfigurationValue(component, shovelerComponentPositionOptionKeyCoordinates);
	return &value->vector3Value;
}

bool shovelerViewEntityRemovePosition(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNamePosition);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove position from entity %lld which does not have a position, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNamePosition);
}
