#ifndef SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H
#define SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H

#include <stdbool.h> // bool

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameTilemapColliders = "tilemap_colliders";
static const char *shovelerComponentTilemapCollidersOptionKeyNumColumns = "num_columns";
static const char *shovelerComponentTilemapCollidersOptionKeyNumRows = "num_rows";
static const char *shovelerComponentTilemapCollidersOptionKeyColliders = "colliders";

ShovelerComponentType *shovelerComponentCreateTilemapColliders();
const bool *shovelerComponentGetTilemapColliders(ShovelerComponent *component);

#endif
