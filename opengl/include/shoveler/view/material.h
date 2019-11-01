#ifndef SHOVELER_VIEW_MATERIAL_H
#define SHOVELER_VIEW_MATERIAL_H

#include <stdbool.h> // bool

#include <shoveler/component/material.h>
#include <shoveler/material.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	ShovelerComponentMaterialType type;
	long long int textureEntityId;
	long long int textureSamplerEntityId;
	long long int tilemapEntityId;
	long long int canvasEntityId;
	long long int chunkEntityId;
	ShovelerVector3 color;
	ShovelerVector2 canvasRegionPosition;
	ShovelerVector2 canvasRegionSize;
} ShovelerViewMaterialConfiguration;

/** Adds a chunk material component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration);
ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity);
/** Returns the current material configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetMaterialConfiguration(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration *outputConfiguration);
/** Updates a material component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration);
bool shovelerViewEntityRemoveMaterial(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetMaterialComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentTypeName);
}

#endif
