#ifndef SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H
#define SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H

#include <stdbool.h> // bool

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewTilemapCollidersComponentTypeName = "tilemap_colliders";
static const char *shovelerViewTilemapCollidersNumColumnsOptionKey = "num_columns";
static const char *shovelerViewTilemapCollidersNumRowsOptionKey = "num_rows";
static const char *shovelerViewTilemapCollidersCollidersOptionKey = "colliders";

ShovelerComponentType *shovelerComponentCreateTilemapColliders();
const bool *shovelerComponentGetTilemapColliders(ShovelerComponent *component);

#endif
