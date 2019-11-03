#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/component/position.h"
#include "shoveler/view/position.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdPosition = "position";

ShovelerComponent *shovelerViewEntityAddPosition(ShovelerViewEntity *entity, ShovelerVector3 coordinates)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdPosition)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreatePositionType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, coordinates);

	shovelerComponentActivate(component);
	return component;
}

const ShovelerVector3 *shovelerViewEntityGetPositionCoordinates(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdPosition);
	if(component == NULL) {
		return NULL;
	}

	const ShovelerComponentConfigurationValue *value = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES);
	return &value->vector3Value;
}

bool shovelerViewEntityRemovePosition(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdPosition);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove position from entity %lld which does not have a position, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdPosition);
}
