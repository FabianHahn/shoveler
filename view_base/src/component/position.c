#include "shoveler/component/position.h"

#include <stdlib.h> // malloc, free

#include "shoveler/component.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdPosition = "position";

static void *activatePositionComponent(ShovelerComponent *component);
static void deactivatePositionComponent(ShovelerComponent *component);
static bool liveUpdatePositionCoordinates(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static bool liveUpdatePositionReleativeParent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static void updatePosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreatePositionType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES] = shovelerComponentTypeConfigurationOption("coordinates", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, liveUpdatePositionCoordinates);
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_ID_RELATIVE_PARENT_POSITION] = shovelerComponentTypeConfigurationOptionDependency("relative_parent_position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, liveUpdatePositionReleativeParent);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdPosition, activatePositionComponent, /* update */ NULL, deactivatePositionComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

const ShovelerVector3 *shovelerComponentGetPosition(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdPosition);

	return component->data;
}

static void *activatePositionComponent(ShovelerComponent *component)
{
	ShovelerComponentPositionType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE);
	bool hasRelativeParentPosition = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_RELATIVE_PARENT_POSITION);

	if(type == SHOVELER_COMPONENT_POSITION_TYPE_RELATIVE && !hasRelativeParentPosition) {
		shovelerLogWarning("Failed to activate entity %lld position because its type is relative but it doesn't have a relative parent position set.", component->entityId);
		return NULL;
	}

	component->data = malloc(sizeof(ShovelerVector3));
	updatePosition(component);

	return component->data;
}

static void deactivatePositionComponent(ShovelerComponent *component)
{
	free(component->data);
}

static bool liveUpdatePositionCoordinates(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	updatePosition(component);

	return true; // propagate update
}

static bool liveUpdatePositionReleativeParent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	updatePosition(component);

	return true; // propagate update
}

void updatePosition(ShovelerComponent *component)
{
	ShovelerVector3 *position = component->data;

	ShovelerComponentPositionType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE);
	ShovelerVector3 coordinates = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES);

	if(type == SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE) {
		*position = coordinates;
	} else {
		ShovelerComponent *relativeParentPositionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_RELATIVE_PARENT_POSITION);
		assert(relativeParentPositionComponent != NULL);
		const ShovelerVector3 *parentPosition = shovelerComponentGetPosition(relativeParentPositionComponent);
		*position = shovelerVector3LinearCombination(1.0f, coordinates, 1.0f, *parentPosition);
	}
}
