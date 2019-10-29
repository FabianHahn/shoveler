#include "shoveler/component/position.h"

#include <string.h>

#include "shoveler/component.h"

ShovelerComponentType *shovelerComponentCreatePositionType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeNamePosition, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentPositionOptionKeyCoordinates, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, shovelerComponentLiveUpdateNoop);

	return componentType;
}

const ShovelerVector3 *shovelerComponentGetPositionCoordinates(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerComponentTypeNamePosition) == 0);

	const ShovelerComponentConfigurationValue *value = shovelerComponentGetConfigurationValue(component, shovelerComponentPositionOptionKeyCoordinates);
	return &value->vector3Value;
}
