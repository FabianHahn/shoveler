#ifndef SHOVELER_COMPONENT_CHUNK_LAYER_H
#define SHOVELER_COMPONENT_CHUNK_LAYER_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewChunkLayerComponentTypeName = "chunk_layer";
static const char *shovelerViewChunkLayerTypeOptionKey = "type";
static const char *shovelerViewChunkLayerCanvasOptionKey = "canvas";
static const char *shovelerViewChunkLayerTilemapOptionKey = "tilemap";

ShovelerComponentType *shovelerComponentCreateChunkLayerType();

#endif
