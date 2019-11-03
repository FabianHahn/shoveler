#ifndef SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H
#define SHOVELER_COMPONENT_TILEMAP_COLLIDERS_H

#include <stdbool.h> // bool

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdTilemapColliders;

typedef enum {
	SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS,
	SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS,
	SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS,
} ShovelerComponentTilemapCollidersOptionId;

ShovelerComponentType *shovelerComponentCreateTilemapColliders();
const bool *shovelerComponentGetTilemapColliders(ShovelerComponent *component);

#endif
