#include "shoveler/component/position.h"

#include "shoveler/component.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdPosition = "position";

static void *activatePositionComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreatePositionType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES] = shovelerComponentTypeConfigurationOption("coordinates", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, shovelerComponentLiveUpdatePropagate);
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_ID_RELATIVE_PARENT_POSITION] = shovelerComponentTypeConfigurationOptionDependency("relative_parent_position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, shovelerComponentLiveUpdateDependencyPropagate);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdPosition, activatePositionComponent, /* update */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerVector3 shovelerComponentGetPositionCoordinates(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdPosition);

	ShovelerComponentPositionType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE);
	ShovelerVector3 coordinates = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES);

	if(type == SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE) {
		return coordinates;
	} else {
		ShovelerComponent *relativeParentPositionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_RELATIVE_PARENT_POSITION);
		assert(relativeParentPositionComponent != NULL);
		ShovelerVector3 parentPositionCoordinates = shovelerComponentGetPositionCoordinates(relativeParentPositionComponent);
		return shovelerVector3LinearCombination(1.0f, coordinates, 1.0f, parentPositionCoordinates);
	}
}

static void *activatePositionComponent(ShovelerComponent *component)
{
	ShovelerComponentPositionType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE);
	bool hasRelativeParentPosition = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_RELATIVE_PARENT_POSITION);

	if(type == SHOVELER_COMPONENT_POSITION_TYPE_RELATIVE && !hasRelativeParentPosition) {
		shovelerLogWarning("Failed to activate entity %lld position because its type is relative but it doesn't have a relative parent position set.", component->entityId);
		return NULL;
	}

	return component;
}
