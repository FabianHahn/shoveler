#ifndef SHOVELER_COMPONENT_CHUNK_LAYER_H
#define SHOVELER_COMPONENT_CHUNK_LAYER_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdChunkLayer;

typedef enum {
	SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TYPE,
	SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_CANVAS,
	SHOVELER_COMPONENT_CHUNK_LAYER_OPTION_TILEMAP,
} ShovelerComponentChunkLayerOptionId;

ShovelerComponentType *shovelerComponentCreateChunkLayerType();

#endif
