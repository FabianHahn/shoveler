#include "shoveler/component/chunk_layer.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component.h"

ShovelerComponentType *shovelerComponentCreateChunkLayerType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeNameChunkLayer, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentChunkLayerOptionKeyType, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentChunkLayerOptionKeyCanvas, shovelerComponentTypeNameCanvas, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentChunkLayerOptionKeyTilemap, shovelerComponentTypeNameTilemap, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}
