#ifndef SHOVELER_COMPONENT_CHUNK_H
#define SHOVELER_COMPONENT_CHUNK_H

typedef struct ShovelerChunkStruct ShovelerChunk; // forward declaration: chunk.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewChunkComponentTypeName = "chunk";
static const char *shovelerViewChunkPositionOptionKey = "position";
static const char *shovelerViewChunkPositionMappingXOptionKey = "position_mapping_x";
static const char *shovelerViewChunkPositionMappingYOptionKey = "position_mapping_y";
static const char *shovelerViewChunkSizeOptionKey = "size";
static const char *shovelerViewChunkLayersOptionKey = "layers";

ShovelerComponentType *shovelerComponentCreateChunkType();
ShovelerChunk *shovelerComponentGetChunk(ShovelerComponent *component);

#endif
