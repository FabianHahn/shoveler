#ifndef SHOVELER_COMPONENT_CHUNK_H
#define SHOVELER_COMPONENT_CHUNK_H

typedef struct ShovelerChunkStruct ShovelerChunk; // forward declaration: chunk.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameChunk = "chunk";
static const char *shovelerComponentChunkOptionKeyPosition = "position";
static const char *shovelerComponentChunkOptionKeyPositionMappingX = "position_mapping_x";
static const char *shovelerComponentChunkOptionKeyPositionMappingY = "position_mapping_y";
static const char *shovelerComponentChunkOptionKeySize = "size";
static const char *shovelerComponentChunkOptionKeyLayers = "layers";

ShovelerComponentType *shovelerComponentCreateChunkType();
ShovelerChunk *shovelerComponentGetChunk(ShovelerComponent *component);

#endif
