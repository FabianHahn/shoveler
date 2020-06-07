#include "shoveler/component/position.h"

#include "shoveler/component.h"

const char *const shovelerComponentTypeIdPosition = "position";

ShovelerComponentType *shovelerComponentCreatePositionType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[1];
	configurationOptions[SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES] = shovelerComponentTypeConfigurationOption("coordinates", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, shovelerComponentLiveUpdatePropagate);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdPosition, /* activate */ NULL, /* update */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

const ShovelerVector3 *shovelerComponentGetPositionCoordinates(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdPosition);

	const ShovelerComponentConfigurationValue *value = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES);
	return &value->vector3Value;
}
