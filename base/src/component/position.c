#include "shoveler/component/position.h"

#include <string.h>

#include "shoveler/component.h"

ShovelerComponentType *shovelerComponentCreatePositionType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[1];
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_COORDINATES] = shovelerComponentTypeConfigurationOption("coordinates", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, shovelerComponentLiveUpdateNoop);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdPosition, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

const ShovelerVector3 *shovelerComponentGetPositionCoordinates(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdPosition) == 0);

	const ShovelerComponentConfigurationValue *value = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_POSITION_OPTION_COORDINATES);
	return &value->vector3Value;
}
