#include "shoveler/component/chunk_layer.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component.h"

const char *const shovelerComponentTypeIdChunkLayer = "chunk_layer";

ShovelerComponentType *shovelerComponentCreateChunkLayerType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_CANVAS] = shovelerComponentTypeConfigurationOptionDependency("canvas", shovelerComponentTypeIdCanvas, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TILEMAP] = shovelerComponentTypeConfigurationOptionDependency("tilemap", shovelerComponentTypeIdTilemap, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdChunkLayer, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}
