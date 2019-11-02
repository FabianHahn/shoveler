#include "shoveler/component/chunk_layer.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component.h"

const char *const shovelerComponentTypeIdChunkLayer = "chunk_layer";

ShovelerComponentType *shovelerComponentCreateChunkLayerType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdChunkLayer, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentChunkLayerOptionKeyType, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentChunkLayerOptionKeyCanvas, shovelerComponentTypeIdCanvas, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentChunkLayerOptionKeyTilemap, shovelerComponentTypeIdTilemap, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}
