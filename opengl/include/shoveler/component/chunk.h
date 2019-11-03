#ifndef SHOVELER_COMPONENT_CHUNK_H
#define SHOVELER_COMPONENT_CHUNK_H

typedef struct ShovelerChunkStruct ShovelerChunk; // forward declaration: chunk.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdChunk;

typedef enum {
	SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION,
	SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_X,
	SHOVELER_COMPONENT_CHUNK_OPTION_ID_POSITION_MAPPING_Y,
	SHOVELER_COMPONENT_CHUNK_OPTION_ID_SIZE,
	SHOVELER_COMPONENT_CHUNK_OPTION_ID_LAYERS,
} ShovelerComponentChunkOptionId;

ShovelerComponentType *shovelerComponentCreateChunkType();
ShovelerChunk *shovelerComponentGetChunk(ShovelerComponent *component);

#endif
