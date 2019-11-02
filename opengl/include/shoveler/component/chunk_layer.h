#ifndef SHOVELER_COMPONENT_CHUNK_LAYER_H
#define SHOVELER_COMPONENT_CHUNK_LAYER_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameChunkLayer = "chunk_layer";
static const char *shovelerComponentChunkLayerOptionKeyType = "type";
static const char *shovelerComponentChunkLayerOptionKeyCanvas = "canvas";
static const char *shovelerComponentChunkLayerOptionKeyTilemap = "tilemap";

ShovelerComponentType *shovelerComponentCreateChunkLayerType();

#endif
