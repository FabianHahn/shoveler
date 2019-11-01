#include "shoveler/component/chunk_layer.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component.h"

ShovelerComponentType *shovelerComponentCreateChunkLayerType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewChunkLayerComponentTypeName, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewChunkLayerTypeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkLayerCanvasOptionKey, shovelerViewCanvasComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewChunkLayerTilemapOptionKey, shovelerViewTilemapComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}
